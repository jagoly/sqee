#include <sqee/vk/VulkTexture.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/vk/Helpers.hpp>

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

    log_error("invalid texture format string '{}'", arg);
}

static vk::SamplerAddressMode impl_string_to_wrap(const String& arg)
{
    if (arg == "Repeat") return vk::SamplerAddressMode::eRepeat;
    if (arg == "MirrorRepeat") return vk::SamplerAddressMode::eMirroredRepeat;
    if (arg == "Clamp") return vk::SamplerAddressMode::eClampToEdge;
    if (arg == "MirrorClamp") return vk::SamplerAddressMode::eMirrorClampToEdge;

    log_error("invalid texture wrap string '{}'", arg);
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

        log_error("invalid texture swizzle char '{}'", c);
    };

    if (arg.size() != 4u)
        log_error("invalid texture swizzle string '{}'", arg);

    const auto swizzleR = char_to_swizzle(arg[0]);
    const auto swizzleG = char_to_swizzle(arg[1]);
    const auto swizzleB = char_to_swizzle(arg[2]);
    const auto swizzleA = char_to_swizzle(arg[3]);

    return { swizzleR, swizzleG, swizzleB, swizzleA };
}

//============================================================================//

inline auto impl_get_format_info(vk::Format format)
{
    struct Result { int channels; size_t pixelSize; bool isSigned; };

    if (format == vk::Format::eR8Srgb) return Result { 1u, 1u, false };
    if (format == vk::Format::eR8G8Srgb) return Result { 2u, 2u, false };
    if (format == vk::Format::eR8G8B8A8Srgb) return Result { 4u, 4u, false };

    if (format == vk::Format::eR8Unorm) return Result { 1u, 1u, false };
    if (format == vk::Format::eR8G8Unorm) return Result { 2u, 2u, false };
    if (format == vk::Format::eR8G8B8A8Unorm) return Result { 4u, 4u, false };

    if (format == vk::Format::eR8Snorm) return Result { 1u, 1u, true };
    if (format == vk::Format::eR8G8Snorm) return Result { 2u, 2u, true };
    if (format == vk::Format::eR8G8B8A8Snorm) return Result { 4u, 4u, true };

    return Result {};
}

//============================================================================//

static auto impl_load_image(vk::Format format, const String& path)
{
    struct Image
    {
        ~Image() { stbi_image_free(data); };
        void* data; Vec2U size;
    };

    const auto formatInfo = impl_get_format_info(format);

    int width, height, channels;
    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, formatInfo.channels);

    if (data == nullptr)
        log_error_multiline("Error loading image '{}':\n{}", path, stbi_failure_reason());

    // todo: allow signed textures that aren't normal maps
    if (formatInfo.isSigned == true)
    {
        int8_t* begin = reinterpret_cast<int8_t*>(data);
        int8_t* end = begin + width * height * formatInfo.channels;

        // convert values to signed twos compliment values from -127 to +127
        for (int8_t* p = begin; p != end; ++p)
            *p += *p >= 0 ? -127 : +128;

        // normal maps have opengl style +Y, so invert it
        for (int8_t* p = begin+1; p != end+1 ; p += formatInfo.channels)
            *p = -*p;
    }

    return Image { data, Vec2U(width, height) };
}

//============================================================================//

VulkTexture::VulkTexture(VulkTexture&& other)
{
    *this = std::move(other);
}

VulkTexture& VulkTexture::operator=(VulkTexture&& other)
{
    std::swap(mImage, other.mImage);
    std::swap(mImageMem, other.mImageMem);
    std::swap(mImageView, other.mImageView);
    std::swap(mSampler, other.mSampler);
    return *this;
}

VulkTexture::~VulkTexture()
{
    const auto& ctx = VulkanContext::get();
    if (mSampler) ctx.device.destroy(mSampler);
    if (mImageView) ctx.device.destroy(mImageView);
    if (mImage) ctx.device.destroy(mImage);
    if (mImageMem) mImageMem.free();
}

//============================================================================//

void VulkTexture::initialise_2D(const Config& config)
{
    SQASSERT(!mImage, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    std::tie(mImage, mImageMem, mImageView) = vk_create_image_2D (
        ctx, config.format, Vec2U(config.size), config.mipLevels, vk::SampleCountFlagBits::e1,
        false, usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_common(config);
}

//============================================================================//

void VulkTexture::load_from_file_2D(const String& path)
{
    const JsonValue json = parse_json_from_file(path + ".json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = impl_string_to_wrap(json.at("wrapX"));
    config.wrapY = impl_string_to_wrap(json.at("wrapY"));
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = json.at("filter");
    config.anisotropy = json.at("anisotropy");

    const auto image = impl_load_image(config.format, path + ".png");
    config.size = Vec3U(image.size, 1u);
    config.mipLevels = json.at("mipmaps") ? 1u + uint(std::floor(std::log2(std::max(config.size.x, config.size.y)))) : 1u;

    initialise_2D(config);
    load_from_memory(image.data, 0u, config);
}

//============================================================================//

void VulkTexture::initialise_array(const Config& config)
{
    SQASSERT(!mImage, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    std::tie(mImage, mImageMem, mImageView) = vk_create_image_array (
        ctx, config.format, config.size, config.mipLevels, vk::SampleCountFlagBits::e1,
        false, usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_common(config);
}

//============================================================================//

void VulkTexture::load_from_file_array(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureArray.json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = impl_string_to_wrap(json.at("wrapX"));
    config.wrapY = impl_string_to_wrap(json.at("wrapY"));
    config.wrapZ = impl_string_to_wrap(json.at("wrapZ"));
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = json.at("filter");
    config.anisotropy = json.at("anisotropy");
    config.size = json.at("size");

    config.mipLevels = json.at("mipmaps") ? 1u + uint(std::floor(std::log2(std::max(config.size.x, config.size.y)))) : 1u;

    initialise_array(config);

    for (uint layer = 0u; layer < config.size.z; ++layer)
    {
        const uint digits = config.size.z > 10u ? config.size.z > 100u ? 3u : 2u : 1u;
        const auto image = impl_load_image(config.format, fmt::format("{}/{:0>{}}.png", path, layer, digits));
        SQASSERT(image.size == Vec2U(config.size), "image size does not match json");
        load_from_memory(image.data, layer, config);
    }
}

//============================================================================//

void VulkTexture::initialise_cube(const Config& config)
{
    SQASSERT(!mImage, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if (config.mipLevels > 1u) usageFlags |= vk::ImageUsageFlagBits::eTransferSrc;

    std::tie(mImage, mImageMem, mImageView) = vk_create_image_cube (
        ctx, config.format, config.size.x, config.mipLevels, vk::SampleCountFlagBits::e1,
        false, usageFlags, false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_common(config);
}

//============================================================================//

void VulkTexture::load_from_file_cube(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureCube.json");

    Config config;
    config.format = impl_string_to_format(json.at("format"));
    config.wrapX = vk::SamplerAddressMode::eRepeat;
    config.wrapY = vk::SamplerAddressMode::eRepeat;
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_string_to_swizzle(json.at("swizzle"));
    config.filter = json.at("filter");
    config.anisotropy = json.at("anisotropy");
    config.size = Vec3U(Vec2U(uint(json.at("size"))), 6u);

    config.mipLevels = json.at("mipmaps") ? 1u + uint(std::floor(std::log2(config.size.x))) : 1u;

    initialise_cube(config);

    for (const char* fname : {"0_right", "1_left", "2_down", "3_up", "4_forward", "5_back"})
    {
        const auto image = impl_load_image(config.format, fmt::format("{}/{}.png", path, fname));
        SQASSERT(image.size == Vec2U(config.size), "image size does not match json");
        load_from_memory(image.data, uint(*fname - '0'), config);
    }
}

//============================================================================//

void VulkTexture::impl_initialise_common(const Config& config)
{
    const auto& ctx = VulkanContext::get();

    mSampler = ctx.device.createSampler (
        vk::SamplerCreateInfo {
            {}, config.filter ? vk::Filter::eLinear : vk::Filter::eNearest,
            config.filter ? vk::Filter::eLinear : vk::Filter::eNearest,
            config.filter ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest,
            config.wrapX, config.wrapY, config.wrapZ,
            0.f, config.anisotropy, ctx.limits.maxAnisotropy,
            false, vk::CompareOp::eAlways, 0.f, 0.f,
            vk::BorderColor::eFloatTransparentBlack, false
        }
    );

    mDescriptorInfo = vk::DescriptorImageInfo(mSampler, mImageView, vk::ImageLayout::eShaderReadOnlyOptimal);
}

//============================================================================//

void VulkTexture::load_from_memory(void* data, uint layer, const Config& config)
{
    const auto& ctx = VulkanContext::get();

    const auto formatInfo = impl_get_format_info(config.format);

    auto staging = StagingBuffer(ctx, config.size.x * config.size.y * formatInfo.pixelSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, config.size.x * config.size.y * formatInfo.pixelSize);
    staging.memory.unmap();

    vk_transition_image_memory_layout (
        cmdbuf.cmdbuf, mImage,
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        vk::ImageAspectFlagBits::eColor, 0u, config.mipLevels, layer, 1u
    );

    cmdbuf->copyBufferToImage (
        staging.buffer, mImage, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy {
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, layer, 1u),
            vk::Offset3D(0, 0, 0), vk::Extent3D(config.size.x, config.size.y, 1u)
        }
    );

    if (config.mipLevels == 1u)
    {
        vk_transition_image_memory_layout (
            cmdbuf.cmdbuf, mImage,
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageAspectFlagBits::eColor, 0u, 1u, layer, 1u
        );
        return; // no mipmap generation
    }

    //--------------------------------------------------------//

    Vec2I sourceSize = Vec2I(config.size.x, config.size.y);

    for (uint i = 1u; i < config.mipLevels; ++i)
    {
        const Vec2I levelSize = maths::max(Vec2I(1, 1), sourceSize / 2);

        vk_transition_image_memory_layout (
            cmdbuf.cmdbuf, mImage,
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageAspectFlagBits::eColor, i - 1u, 1u, layer, 1u
        );

        cmdbuf->blitImage (
            mImage, vk::ImageLayout::eTransferSrcOptimal, mImage, vk::ImageLayout::eTransferDstOptimal,
            vk::ImageBlit {
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1u, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(sourceSize.x, sourceSize.y, 1) },
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(levelSize.x, levelSize.y, 1) }
            },
            vk::Filter::eLinear
        );

        vk_transition_image_memory_layout (
            cmdbuf.cmdbuf, mImage,
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eTransferRead, vk::AccessFlags(),
            vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageAspectFlagBits::eColor, i - 1u, 1u, layer, 1u
        );

        sourceSize = levelSize;
    }

    vk_transition_image_memory_layout (
        cmdbuf.cmdbuf, mImage,
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageAspectFlagBits::eColor, config.mipLevels - 1u, 1u, layer, 1u
    );
}
