#include <sqee/objects/Texture.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/vk/Helpers.hpp>

#include <sqee/redist/stb_image.hpp>

using namespace sq;

//============================================================================//

static vk::Format impl_string_to_format(const String& arg)
{
    // one and two channel srgb textures don't support image blit
    // this means we can't easily generate mipmaps for them
    // this is fine, you want unorm for non-colour values anyway

    if (arg == "4x8_Srgb") return vk::Format::eR8G8B8A8Srgb;

    if (arg == "1x8_Unorm") return vk::Format::eR8Unorm;
    if (arg == "2x8_Unorm") return vk::Format::eR8G8Unorm;
    if (arg == "4x8_Unorm") return vk::Format::eR8G8B8A8Unorm;

    if (arg == "1x8_Snorm") return vk::Format::eR8Snorm;
    if (arg == "2x8_Snorm") return vk::Format::eR8G8Snorm;
    if (arg == "4x8_Snorm") return vk::Format::eR8G8B8A8Snorm;

    if (arg == "5999_Hdr") return vk::Format::eE5B9G9R9UfloatPack32;

    SQEE_THROW("invalid texture format string '{}'", arg);
}

static vk::SamplerAddressMode impl_string_to_wrap(const String& arg)
{
    if (arg == "Repeat") return vk::SamplerAddressMode::eRepeat;
    if (arg == "MirrorRepeat") return vk::SamplerAddressMode::eMirroredRepeat;
    if (arg == "Clamp") return vk::SamplerAddressMode::eClampToEdge;
    if (arg == "MirrorClamp") return vk::SamplerAddressMode::eMirrorClampToEdge;

    SQEE_THROW("invalid texture wrap string '{}'", arg);
}

static vk::ComponentMapping impl_string_to_swizzle(const String& arg)
{
    const auto char_to_swizzle = [](char c)
    {
        if (c == '0') return vk::ComponentSwizzle::eZero;
        if (c == '1') return vk::ComponentSwizzle::eOne;
        if (c == 'R') return vk::ComponentSwizzle::eR;
        if (c == 'G') return vk::ComponentSwizzle::eG;
        if (c == 'B') return vk::ComponentSwizzle::eB;
        if (c == 'A') return vk::ComponentSwizzle::eA;

        SQEE_THROW("invalid texture swizzle char '{}'", c);
    };

    if (arg.size() != 4u)
        SQEE_THROW("invalid texture swizzle string '{}'", arg);

    const auto swizzleR = char_to_swizzle(arg[0]);
    const auto swizzleG = char_to_swizzle(arg[1]);
    const auto swizzleB = char_to_swizzle(arg[2]);
    const auto swizzleA = char_to_swizzle(arg[3]);

    return { swizzleR, swizzleG, swizzleB, swizzleA };
}

static Texture::FilterMode impl_string_to_filter(const String& arg)
{
    if (arg == "Nearest") return Texture::FilterMode::Nearest;
    if (arg == "Linear") return Texture::FilterMode::Linear;
    if (arg == "Anisotropic") return Texture::FilterMode::Anisotropic;

    SQEE_THROW("invalid texture mipmaps string '{}'", arg);
}

static Texture::MipmapsMode impl_string_to_mipmaps(const String& arg)
{
    if (arg == "Disable") return Texture::MipmapsMode::Disable;
    if (arg == "Generate") return Texture::MipmapsMode::Generate;
    if (arg == "Load") return Texture::MipmapsMode::Load;

    SQEE_THROW("invalid texture mipmaps string '{}'", arg);
}

//============================================================================//

inline auto impl_get_format_info(vk::Format format)
{
    struct Result { int channels; size_t pixelSize; bool isSigned; };

    if (format == vk::Format::eR8Srgb) return Result { 1, 1, false };
    if (format == vk::Format::eR8G8Srgb) return Result { 2, 2, false };
    if (format == vk::Format::eR8G8B8A8Srgb) return Result { 4, 4, false };

    if (format == vk::Format::eR8Unorm) return Result { 1, 1, false };
    if (format == vk::Format::eR8G8Unorm) return Result { 2, 2, false };
    if (format == vk::Format::eR8G8B8A8Unorm) return Result { 4, 4, false };

    if (format == vk::Format::eR8Snorm) return Result { 1, 1, true };
    if (format == vk::Format::eR8G8Snorm) return Result { 2, 2, true };
    if (format == vk::Format::eR8G8B8A8Snorm) return Result { 4, 4, true };

    if (format == vk::Format::eE5B9G9R9UfloatPack32) return Result { 3, 4, false };

    if (format == vk::Format::eR16Unorm) return Result { 1, 2, false };
    if (format == vk::Format::eR16G16Unorm) return Result { 2, 4, false };
    if (format == vk::Format::eR16G16B16A16Unorm) return Result { 4, 8, false };

    return Result {};
}

//============================================================================//

inline uint32_t vec3_to_e5bgr9(Vec3F rgb)
{
    constexpr float MAX_VALUE = float((1<<9)-1) / float(1<<9) * float(1<<5);

    const Vec3F clamped = maths::clamp(rgb, Vec3F(0.f), Vec3F(MAX_VALUE));
    const float maxComponent = maths::max(clamped.r, clamped.g, clamped.b);

    const float prelimExponent = std::max(-15.f - 1.f, std::floor(std::log2(maxComponent))) + 1.f + 15.f;
    const float maxExponent = std::floor(maxComponent / std::pow(2.f, (prelimExponent - 15.f - 9.f)) + 0.5f);
    const float exponent = maxExponent < std::pow(2.f, 9.f) ? prelimExponent : prelimExponent + 1.f;

    const Vec3U values = Vec3U(clamped / std::pow(2.f, (exponent - 15.f - 9.f)) + 0.5f);

    return uint(exponent) << 27 | values.b << 18 | values.g << 9 | values.r;
}

//============================================================================//

static auto impl_load_image(vk::Format format, const String& path)
{
    struct Image
    {
        ~Image() { std::free(data); };
        void* data; Vec2U size;
    };

    const auto formatInfo = impl_get_format_info(format);
    const auto bytes = read_bytes_from_file(path);

    int width, height, channels;

    if (format == vk::Format::eE5B9G9R9UfloatPack32)
    {
        float* floats = stbi_loadf_from_memory (
            reinterpret_cast<const uint8_t*>(bytes.data()), int(bytes.size()),
            &width, &height, &channels, formatInfo.channels
        );

        if (floats == nullptr)
            SQEE_THROW("image load failure: {}", stbi_failure_reason());

        // use malloc so that we can use free instead of delete
        uint32_t* data = static_cast<uint32_t*>(std::malloc(width * height * sizeof(uint32_t)));

        std::transform (
            reinterpret_cast<Vec3F*>(floats), reinterpret_cast<Vec3F*>(floats) + width * height,
            data, vec3_to_e5bgr9
        );

        std::free(floats);

        return Image { data, Vec2U(width, height) };
    }
    else
    {
        SQASSERT(formatInfo.channels == int(formatInfo.pixelSize), "todo: loading 16bit images");

        uint8_t* data = stbi_load_from_memory (
            reinterpret_cast<const uint8_t*>(bytes.data()), int(bytes.size()),
            &width, &height, &channels, formatInfo.channels
        );

        if (data == nullptr)
            SQEE_THROW("image load failure: {}", stbi_failure_reason());

        // todo: allow signed textures that aren't normal maps
        if (formatInfo.isSigned == true)
        {
            int8_t* const begin = reinterpret_cast<int8_t*>(data);
            int8_t* const end = begin + width * height * formatInfo.channels;

            // convert values to signed twos compliment values from -127 to +127
            for (int8_t* p = begin; p != end; ++p)
                *p += *p >= 0 ? -127 : +128;

            // normal maps have opengl style +Y, so invert it
            for (int8_t* p = begin+1; p != end+1; p += formatInfo.channels)
                *p = -*p;
        }

        return Image { data, Vec2U(width, height) };
    }
}

//============================================================================//

Texture::Texture(Texture&& other)
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other)
{
    std::swap(mStuff, other.mStuff);
    std::swap(mSampler, other.mSampler);
    return *this;
}

Texture::~Texture()
{
    if (!mStuff.image) return;
    const auto& ctx = VulkanContext::get();
    mStuff.destroy(ctx);
    ctx.device.destroy(mSampler);
}

//============================================================================//

void Texture::initialise_2D(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    mStuff.initialise_2D (
        ctx, config.format, Vec2U(config.size), config.mipLevels, vk::SampleCountFlagBits::e1,
        usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_sampler(config);
}

//============================================================================//

void Texture::load_from_file_2D(const String& path)
{
    const JsonValue json = parse_json_from_file(path + ".json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = impl_string_to_wrap(json.at("wrapX"));
    config.wrapY = impl_string_to_wrap(json.at("wrapY"));
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = impl_string_to_filter(json.at("filter"));
    config.mipmaps = impl_string_to_mipmaps(json.at("mipmaps"));

    const auto image = impl_load_image(config.format, path + ".png");
    config.size = Vec3U(image.size, 1u);

    if (config.mipmaps == MipmapsMode::Disable) config.mipLevels = 1u;
    else config.mipLevels = 1u + uint(std::floor(std::log2(std::max(config.size.x, config.size.y))));

    SQASSERT(config.mipmaps != MipmapsMode::Load, "todo: 2D mipmap load");

    initialise_2D(config);
    load_from_memory(image.data, 0u, 0u, config);
}

//============================================================================//

void Texture::initialise_array(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    mStuff.initialise_array (
        ctx, config.format, config.size, config.mipLevels, vk::SampleCountFlagBits::e1,
        usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_sampler(config);
}

//============================================================================//

void Texture::load_from_file_array(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureArray.json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = impl_string_to_wrap(json.at("wrapX"));
    config.wrapY = impl_string_to_wrap(json.at("wrapY"));
    config.wrapZ = impl_string_to_wrap(json.at("wrapZ"));
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = impl_string_to_filter(json.at("filter"));
    config.mipmaps = impl_string_to_mipmaps(json.at("mipmaps"));
    config.size = json.at("size");

    if (config.mipmaps == MipmapsMode::Disable) config.mipLevels = 1u;
    else config.mipLevels = 1u + uint(std::floor(std::log2(std::max(config.size.x, config.size.y))));

    SQASSERT(config.mipmaps != MipmapsMode::Load, "todo: array mipmap load");

    initialise_array(config);

    const auto formatStr = config.format == vk::Format::eE5B9G9R9UfloatPack32 ? "{}/{:0>{}}.hdr" : "{}/{:0>{}}.png";
    const uint digits = config.size.z > 10u ? config.size.z > 100u ? 3u : 2u : 1u;

    for (uint layer = 0u; layer < config.size.z; ++layer)
    {
        const auto image = impl_load_image(config.format, fmt::format(formatStr, path, layer, digits));
        SQASSERT(image.size == Vec2U(config.size), "image size does not match json");
        load_from_memory(image.data, 0u, layer, config);
    }
}

//============================================================================//

void Texture::initialise_cube(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    mStuff.initialise_cube (
        ctx, config.format, config.size.x, config.mipLevels, vk::SampleCountFlagBits::e1,
        usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_sampler(config);
}

//============================================================================//

void Texture::load_from_file_cube(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureCube.json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = vk::SamplerAddressMode::eRepeat;
    config.wrapY = vk::SamplerAddressMode::eRepeat;
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = impl_string_to_filter(json.at("filter"));
    config.mipmaps = impl_string_to_mipmaps(json.at("mipmaps"));
    config.size = Vec3U(Vec2U(uint(json.at("size"))), 6u);

    if (config.mipmaps == MipmapsMode::Disable) config.mipLevels = 1u;
    else config.mipLevels = 1u + uint(std::floor(std::log2(config.size.x)));

    initialise_cube(config);

    if (config.mipmaps == MipmapsMode::Load)
    {
        const auto formatStr = config.format == vk::Format::eE5B9G9R9UfloatPack32 ? "{}/{}/{}.hdr" : "{}/{}/{}.png";
        for (const auto faceName : {"0_right", "1_left", "2_down", "3_up", "4_forward", "5_back"})
        {
            for (uint level = 0u; level < config.mipLevels; ++level)
            {
                const auto image = impl_load_image(config.format, fmt::format(formatStr, path, level, faceName));
                SQASSERT(image.size == Vec2U(config.size) / uint(std::exp2(level)), "image size does not match level");
                load_from_memory(image.data, level, uint(*faceName - '0'), config);
            }
        }
    }
    else // disabled or generated mipmaps
    {
        const auto formatStr = config.format == vk::Format::eE5B9G9R9UfloatPack32 ? "{}/{}.hdr" : "{}/{}.png";
        for (const auto faceName : {"0_right", "1_left", "2_down", "3_up", "4_forward", "5_back"})
        {
            const auto image = impl_load_image(config.format, fmt::format(formatStr, path, faceName));
            SQASSERT(image.size == Vec2U(config.size), "image size does not match json");
            load_from_memory(image.data, 0u, uint(*faceName - '0'), config);
        }
    }
}

//============================================================================//

void Texture::impl_initialise_sampler(const Config& config)
{
    const auto& ctx = VulkanContext::get();

    mSampler = ctx.create_sampler (
        config.filter != FilterMode::Nearest ? vk::Filter::eLinear : vk::Filter::eNearest,
        config.filter != FilterMode::Nearest ? vk::Filter::eLinear : vk::Filter::eNearest,
        config.filter != FilterMode::Nearest ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest,
        config.wrapX, config.wrapY, config.wrapZ,
        0.f, 0u, config.mipLevels - 1u,
        config.filter == FilterMode::Anisotropic, false
    );
}

//============================================================================//

void Texture::load_from_memory(void* data, uint level, uint layer, const Config& config)
{
    SQASSERT(level == 0u || config.mipmaps != MipmapsMode::Generate, "cannot manually load generated mipmaps");

    const auto& ctx = VulkanContext::get();

    const auto formatInfo = impl_get_format_info(config.format);
    const Vec2U size = Vec2U(config.size) / uint(std::exp2(level));

    auto staging = StagingBuffer(ctx, size.x * size.y * formatInfo.pixelSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, size.x * size.y * formatInfo.pixelSize);
    staging.memory.unmap();

    vk_pipeline_barrier_image_memory (
        cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        vk::ImageAspectFlagBits::eColor, level,
        config.mipmaps == MipmapsMode::Generate ? config.mipLevels : 1u, layer, 1u
    );

    cmdbuf->copyBufferToImage (
        staging.buffer, mStuff.image, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy {
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, layer, 1u),
            vk::Offset3D(0, 0, 0), vk::Extent3D(size.x, size.y, 1u)
        }
    );

    if (config.mipmaps != MipmapsMode::Generate)
    {
        vk_pipeline_barrier_image_memory (
            cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageAspectFlagBits::eColor, level, 1u, layer, 1u
        );
        return; // no mipmap generation
    }

    //--------------------------------------------------------//

    Vec2I sourceSize = Vec2I(config.size.x, config.size.y);

    for (uint i = 1u; i < config.mipLevels; ++i)
    {
        const Vec2I levelSize = maths::max(Vec2I(1, 1), sourceSize / 2);

        vk_pipeline_barrier_image_memory (
            cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageAspectFlagBits::eColor, i - 1u, 1u, layer, 1u
        );

        cmdbuf->blitImage (
            mStuff.image, vk::ImageLayout::eTransferSrcOptimal, mStuff.image, vk::ImageLayout::eTransferDstOptimal,
            vk::ImageBlit {
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1u, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(sourceSize.x, sourceSize.y, 1) },
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(levelSize.x, levelSize.y, 1) }
            },
            vk::Filter::eLinear
        );

        vk_pipeline_barrier_image_memory (
            cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eTransferRead, vk::AccessFlags(),
            vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageAspectFlagBits::eColor, i - 1u, 1u, layer, 1u
        );

        sourceSize = levelSize;
    }

    vk_pipeline_barrier_image_memory (
        cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageAspectFlagBits::eColor, config.mipLevels - 1u, 1u, layer, 1u
    );
}
