#include <sqee/objects/Texture.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Colours.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

#include <sqee/redist/stb_image.hpp>
#include <lz4frame.h>

using namespace sq;

//==============================================================================

static vk::Format impl_json_to_format(JsonAny json)
{
    // one and two channel srgb textures don't support image blit
    // this means we can't easily generate mipmaps for them
    // this is fine, you want unorm for non-colour values anyway

    const auto sv = json.as<StringView>();

    if (sv == "4x8_Srgb") return vk::Format::eR8G8B8A8Srgb;

    if (sv == "1x8_Unorm") return vk::Format::eR8Unorm;
    if (sv == "2x8_Unorm") return vk::Format::eR8G8Unorm;
    if (sv == "4x8_Unorm") return vk::Format::eR8G8B8A8Unorm;

    if (sv == "1x8_Snorm") return vk::Format::eR8Snorm;
    if (sv == "2x8_Snorm") return vk::Format::eR8G8Snorm;
    if (sv == "4x8_Snorm") return vk::Format::eR8G8B8A8Snorm;

    if (sv == "1x16_Unorm") return vk::Format::eR16Unorm;
    if (sv == "2x16_Unorm") return vk::Format::eR16G16Unorm;
    if (sv == "4x16_Unorm") return vk::Format::eR16G16B16A16Unorm;

    if (sv == "5999_Hdr") return vk::Format::eE5B9G9R9UfloatPack32;

    json.throw_with_context("invalid image format");
}

static vk::SamplerAddressMode impl_json_to_wrap(JsonAny json)
{
    const auto sv = json.as<StringView>();

    if (sv == "Repeat") return vk::SamplerAddressMode::eRepeat;
    if (sv == "MirrorRepeat") return vk::SamplerAddressMode::eMirroredRepeat;
    if (sv == "Clamp") return vk::SamplerAddressMode::eClampToEdge;
    if (sv == "MirrorClamp") return vk::SamplerAddressMode::eMirrorClampToEdge;

    json.throw_with_context("invalid wrap mode");
}

static vk::ComponentMapping impl_json_to_swizzle(JsonAny json)
{
    const auto char_to_swizzle = [&](char c)
    {
        if (c == '0') return vk::ComponentSwizzle::eZero;
        if (c == '1') return vk::ComponentSwizzle::eOne;
        if (c == 'R') return vk::ComponentSwizzle::eR;
        if (c == 'G') return vk::ComponentSwizzle::eG;
        if (c == 'B') return vk::ComponentSwizzle::eB;
        if (c == 'A') return vk::ComponentSwizzle::eA;

        json.throw_with_context("invalid swizzle char");
    };

    const auto sv = json.as<StringView>();

    if (sv.length() != 4u)
        json.throw_with_context("invalid swizzle string");

    const auto swizzleR = char_to_swizzle(sv[0]);
    const auto swizzleG = char_to_swizzle(sv[1]);
    const auto swizzleB = char_to_swizzle(sv[2]);
    const auto swizzleA = char_to_swizzle(sv[3]);

    return { swizzleR, swizzleG, swizzleB, swizzleA };
}

static Texture::FilterMode impl_json_to_filter(JsonAny json)
{
    const auto sv = json.as<StringView>();

    if (sv == "Nearest") return Texture::FilterMode::Nearest;
    if (sv == "Linear") return Texture::FilterMode::Linear;
    if (sv == "Anisotropic") return Texture::FilterMode::Anisotropic;

    json.throw_with_context("invalid filter mode");
}

static Texture::MipmapsMode impl_json_to_mipmaps(JsonAny json)
{
    const auto sv = json.as<StringView>();

    if (sv == "Disable") return Texture::MipmapsMode::Disable;
    if (sv == "Generate") return Texture::MipmapsMode::Generate;
    if (sv == "Load") return Texture::MipmapsMode::Load;

    json.throw_with_context("invalid mipmaps mode");
}

//==============================================================================

static auto impl_get_format_info(vk::Format format)
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

    if (format == vk::Format::eR16Unorm) return Result { 1, 2, false };
    if (format == vk::Format::eR16G16Unorm) return Result { 2, 4, false };
    if (format == vk::Format::eR16G16B16A16Unorm) return Result { 4, 8, false };

    if (format == vk::Format::eE5B9G9R9UfloatPack32) return Result { 3, 4, false };

    SQEE_THROW("invalid format '{}'", vk::to_string(format));
}

//==============================================================================

static auto impl_load_image(vk::Format format, const String& path)
{
    struct Image
    {
        ~Image() { std::free(data); };
        void* data; size_t length; Vec2U size;
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

        // use malloc to match stb_image allocations
        void* data = std::malloc(width * height * sizeof(uint32_t));

        std::transform (
            reinterpret_cast<Vec3F*>(floats), reinterpret_cast<Vec3F*>(floats) + width * height,
            static_cast<uint32_t*>(data), maths::hdr_to_e5bgr9
        );

        std::free(floats);

        return Image { data, width * height * formatInfo.pixelSize, Vec2U(width, height) };
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

        return Image { data, width * height * formatInfo.pixelSize, Vec2U(width, height) };
    }
}

//==============================================================================

static vk::Sampler impl_create_sampler(const VulkanContext& ctx, const Texture::Config& config)
{
    return ctx.create_sampler (
        config.filter != Texture::FilterMode::Nearest ? vk::Filter::eLinear : vk::Filter::eNearest,
        config.filter != Texture::FilterMode::Nearest ? vk::Filter::eLinear : vk::Filter::eNearest,
        config.filter != Texture::FilterMode::Nearest ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest,
        config.wrapX, config.wrapY, config.wrapZ,
        0.f, 0u, config.mipLevels - 1u,
        config.filter == Texture::FilterMode::Anisotropic, false, {}
    );
}

//==============================================================================

Texture::Texture(Texture&& other)
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other)
{
    std::swap(mStuff, other.mStuff);
    std::swap(mSampler, other.mSampler);
    mBindlessDescriptorIndex = other.mBindlessDescriptorIndex;
    return *this;
}

Texture::~Texture()
{
    if (!mStuff.image) return;
    const auto& ctx = VulkanContext::get();
    mStuff.destroy(ctx);
    ctx.device.destroy(mSampler);
}

//==============================================================================

void Texture::initialise_2D(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    mStuff.initialise_2D (
        ctx, config.format, Vec2U(config.size), config.mipLevels, vk::SampleCountFlagBits::e1,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    mSampler = impl_create_sampler(ctx, config);
}

//==============================================================================

void Texture::load_from_file_2D(const String& path)
{
    const auto document = JsonDocument::parse_file(path + ".json");
    const auto json = document.root().as<JsonObject>();

    Config config;
    config.format = impl_json_to_format(json["format"]);
    config.wrapX = impl_json_to_wrap(json["wrapX"]);
    config.wrapY = impl_json_to_wrap(json["wrapY"]);
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_json_to_swizzle(json["swizzle"]);
    config.filter = impl_json_to_filter(json["filter"]);
    config.mipmaps = impl_json_to_mipmaps(json["mipmaps"]);
    config.size = Vec3U(json["size"].as<Vec2U>(), 1u);

    if (config.mipmaps != MipmapsMode::Disable)
    {
        if (const auto jMipLevels = json.get_safe("mipLevels")) config.mipLevels = jMipLevels->as<uint>();
        else config.mipLevels = 1u + uint(std::log2(std::max(config.size.x, config.size.y)));
    }
    else config.mipLevels = 1u;

    initialise_2D(config);

    if (try_load_from_compressed(path, config))
        return; // success

    if (config.mipmaps == MipmapsMode::Load)
        json.throw_with_context("mipmaps can only be loaded from lz4 archives");

    const auto image = impl_load_image(config.format, path + ".png");

    if (image.size != Vec2U(config.size))
        json.throw_with_context("size does not match loaded image");

    load_from_memory(image.data, image.length, 0u, 0u, config);
}

//==============================================================================

void Texture::initialise_array(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    mStuff.initialise_array (
        ctx, config.format, config.size, config.mipLevels, vk::SampleCountFlagBits::e1,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    mSampler = impl_create_sampler(ctx, config);
}

//==============================================================================

void Texture::load_from_file_array(const String& path)
{
    const auto document = JsonDocument::parse_file(path + ".json");
    const auto json = document.root().as<JsonObject>();

    Config config;
    config.format = impl_json_to_format(json["format"]);
    config.wrapX = impl_json_to_wrap(json["wrapX"]);
    config.wrapY = impl_json_to_wrap(json["wrapY"]);
    config.wrapZ = impl_json_to_wrap(json["wrapZ"]);
    config.swizzle = impl_json_to_swizzle(json["swizzle"]);
    config.filter = impl_json_to_filter(json["filter"]);
    config.mipmaps = impl_json_to_mipmaps(json["mipmaps"]);
    config.size = json["size"].as<Vec3U>();

    if (config.mipmaps != MipmapsMode::Disable)
    {
        if (const auto jMipLevels = json.get_safe("mipLevels")) config.mipLevels = jMipLevels->as<uint>();
        else config.mipLevels = 1u + uint(std::log2(std::max(config.size.x, config.size.y)));
    }
    else config.mipLevels = 1u;

    initialise_array(config);

    if (try_load_from_compressed(path, config))
        return; // success

    if (config.mipmaps == MipmapsMode::Load)
        json.throw_with_context("mipmaps can only be loaded from lz4 archives");

    const auto extension = config.format == vk::Format::eE5B9G9R9UfloatPack32 ? "hdr" : "png";
    const uint digits = config.size.z > 10u ? config.size.z > 100u ? 3u : 2u : 1u;

    for (uint layer = 0u; layer < config.size.z; ++layer)
    {
        const auto image = impl_load_image(config.format, fmt::format("{}/{:0>{}}.{}", path, layer, digits, extension));

        if (image.size != Vec2U(config.size))
            json.throw_with_context("size does not match loaded image for layer {}", layer);

        load_from_memory(image.data, image.length, 0u, layer, config);
    }
}

//==============================================================================

void Texture::initialise_cube(const Config& config)
{
    SQASSERT(!mStuff.image, "texture already loaded");

    const auto& ctx = VulkanContext::get();

    mStuff.initialise_cube (
        ctx, config.format, config.size.x, config.mipLevels, vk::SampleCountFlagBits::e1,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    mSampler = impl_create_sampler(ctx, config);
}

//==============================================================================

void Texture::load_from_file_cube(const String& path)
{
    const auto document = JsonDocument::parse_file(path + ".json");
    const auto json = document.root().as<JsonObject>();

    Config config;
    config.format = impl_json_to_format(json["format"]);
    config.wrapX = vk::SamplerAddressMode::eClampToEdge;
    config.wrapY = vk::SamplerAddressMode::eClampToEdge;
    config.wrapZ = vk::SamplerAddressMode::eRepeat;
    config.swizzle = impl_json_to_swizzle(json["swizzle"]);
    config.filter = impl_json_to_filter(json["filter"]);
    config.mipmaps = impl_json_to_mipmaps(json["mipmaps"]);
    config.size = Vec3U(Vec2U(json["size"].as<uint>()), 6u);

    if (config.mipmaps != MipmapsMode::Disable)
    {
        if (const auto jMipLevels = json.get_safe("mipLevels")) config.mipLevels = jMipLevels->as<uint>();
        else config.mipLevels = 1u + uint(std::log2(config.size.x));
    }
    else config.mipLevels = 1u;

    initialise_cube(config);

    if (try_load_from_compressed(path, config))
        return; // success

    if (config.mipmaps == MipmapsMode::Load)
        json.throw_with_context("mipmaps can only be loaded from lz4 archives");

    const auto extension = config.format == vk::Format::eE5B9G9R9UfloatPack32 ? "hdr" : "png";
    const auto faceNames = std::array { "0_right", "1_left", "2_down", "3_up", "4_forward", "5_back"};

    for (uint face = 0u; face < 6u; ++face)
    {
        const auto image = impl_load_image(config.format, fmt::format("{}/{}.{}", path, faceNames[face], extension));

        if (image.size != Vec2U(config.size))
            json.throw_with_context("size does not match loaded image for face {}", face);

        load_from_memory(image.data, image.length, 0u, face, config);
    }
}

//==============================================================================

void Texture::load_from_memory(const void* data, size_t length, uint level, uint layer, const Config& config)
{
    SQASSERT(level == 0u || config.mipmaps != MipmapsMode::Generate, "cannot manually load generated mipmaps");

    const auto& ctx = VulkanContext::get();

    const Vec2U size = Vec2U(config.size) / uint(std::exp2(level));
    const size_t pixelSize = impl_get_format_info(config.format).pixelSize;
    const size_t bufferSize = size.x * size.y * pixelSize;

    SQASSERT(bufferSize == length, "length does not match expectation");

    auto staging = StagingBuffer(ctx, true, false, bufferSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, bufferSize);
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
        vk::BufferImageCopy (
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, layer, 1u),
            vk::Offset3D(0, 0, 0), vk::Extent3D(size.x, size.y, 1u)
        )
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

    //----------------------------------------------------------

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
            vk::ImageBlit (
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1u, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(sourceSize.x, sourceSize.y, 1) },
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, layer, 1u),
                std::array { vk::Offset3D(0, 0, 0), vk::Offset3D(levelSize.x, levelSize.y, 1) }
            ),
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

//==============================================================================

void Texture::load_from_memory(const void* data, size_t length, const Config& config)
{
    SQASSERT(config.mipmaps != MipmapsMode::Generate, "cannot manually load generated mipmaps");

    const auto& ctx = VulkanContext::get();

    const size_t pixelSize = impl_get_format_info(config.format).pixelSize;
    const size_t bufferSize = compute_buffer_size(config.size, config.mipLevels, pixelSize);

    SQASSERT(bufferSize == length, "length does not match expectation");

    auto staging = StagingBuffer(ctx, true, false, bufferSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, bufferSize);
    staging.memory.unmap();

    vk_pipeline_barrier_image_memory (
        cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        vk::ImageAspectFlagBits::eColor, 0u, config.mipLevels, 0u, config.size.z
    );

    size_t levelBufferOffset = 0u;
    Vec2U levelSize = Vec2U(config.size.x, config.size.y);

    for (uint level = 0u; level < config.mipLevels; ++level)
    {
        cmdbuf->copyBufferToImage (
            staging.buffer, mStuff.image, vk::ImageLayout::eTransferDstOptimal,
            vk::BufferImageCopy (
                levelBufferOffset, 0u, 0u,
                vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, 0u, config.size.z),
                vk::Offset3D(0, 0, 0), vk::Extent3D(levelSize.x, levelSize.y, 1u)
            )
        );

        levelBufferOffset += levelSize.x * levelSize.y * config.size.z * pixelSize;
        levelSize = maths::max(Vec2U(1u, 1u), levelSize / 2u);
    }

    vk_pipeline_barrier_image_memory (
        cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageAspectFlagBits::eColor, 0u, config.mipLevels, 0u, config.size.z
    );
}

//==============================================================================

bool Texture::try_load_from_compressed(const String& path, const Config& config)
{
    const auto compressed = try_read_bytes_from_file(path + ".lz4");
    if (compressed.has_value() == false)
        return false; // fall back to regular image files

    const size_t pixelSize = impl_get_format_info(config.format).pixelSize;
    const size_t decompressedSize = compute_buffer_size(config.size, config.mipLevels, pixelSize);

    // wrapped in a unique_ptr so we can throw exceptions
    auto dctx = []() {
        LZ4F_dctx* raw; LZ4F_createDecompressionContext(&raw, LZ4F_VERSION);
        return std::unique_ptr<LZ4F_dctx, decltype(&LZ4F_freeDecompressionContext)>(raw, &LZ4F_freeDecompressionContext);
    }();

    auto decompressed = std::unique_ptr<std::byte[]>(new std::byte[decompressedSize]());
    {
        size_t dstSize = decompressedSize;
        size_t srcSize = compressed->size();

        if ( auto error = LZ4F_decompress(dctx.get(), decompressed.get(), &dstSize, compressed->data(), &srcSize, nullptr);
             LZ4F_isError(error) )
            SQEE_THROW("'{}.lz4': Decompression error ({})", path, LZ4F_getErrorName(error));

        if (dstSize < decompressedSize)
            SQEE_THROW("'{}.lz4': Decompressed data has incorrect size (too small)", path);

        if (srcSize < compressed->size())
            SQEE_THROW("'{}.lz4': Decompressed data has incorrect size (too large)", path);
    }

    load_from_memory(decompressed.get(), decompressedSize, config);

    return true;
}

//==============================================================================

void Texture::save_as_compressed(const String& path, vk::Format format, Vec3U size, uint mipLevels) const
{
    const auto& ctx = VulkanContext::get();

    const size_t pixelSize = impl_get_format_info(format).pixelSize;
    const size_t bufferSize = compute_buffer_size(size, mipLevels, pixelSize);

    // copy all mip levels into a host buffer
    auto buffer = StagingBuffer(ctx, false, true, bufferSize);
    {
        auto cmdbuf = OneTimeCommands(ctx);

        vk_pipeline_barrier_image_memory (
            cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
            vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlags(), vk::AccessFlagBits::eTransferRead,
            vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageAspectFlagBits::eColor, 0u, mipLevels, 0u, size.z
        );

        size_t levelBufferOffset = 0u;
        Vec2U levelSize = Vec2U(size.x, size.y);

        for (uint level = 0u; level < mipLevels; ++level)
        {
            cmdbuf->copyImageToBuffer (
                mStuff.image, vk::ImageLayout::eTransferSrcOptimal, buffer.buffer,
                vk::BufferImageCopy (
                    levelBufferOffset, 0u, 0u,
                    vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, level, 0u, size.z),
                    vk::Offset3D(0, 0, 0), vk::Extent3D(levelSize.x, levelSize.y, 1u)
                )
            );

            levelBufferOffset += levelSize.x * levelSize.y * size.z * pixelSize;
            levelSize = maths::max(Vec2U(1u, 1u), levelSize / 2u);
        }

        vk_pipeline_barrier_image_memory (
            cmdbuf.cmdbuf, mStuff.image, vk::DependencyFlags(),
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            vk::AccessFlagBits::eTransferRead, vk::AccessFlags(),
            vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageAspectFlagBits::eColor, 0u, mipLevels, 0u, size.z
        );
    }

    // compress the data with lz4 and save it to a file
    {
        LZ4F_preferences_t preferences = LZ4F_INIT_PREFERENCES;
        preferences.compressionLevel = 9;

        const size_t maxCompressedSize = LZ4F_compressFrameBound(bufferSize, &preferences);
        std::byte* compressed = new std::byte[maxCompressedSize];

        const size_t compressedSize = LZ4F_compressFrame (
            compressed, maxCompressedSize, buffer.memory.map(), bufferSize, &preferences
        );
        buffer.memory.unmap();

        write_bytes_to_file(path, compressed, compressedSize);
        delete[] compressed;
    }
}

//==============================================================================

size_t Texture::compute_buffer_size(Vec3U size, uint mipLevels, size_t pixelSize)
{
    size_t bufferTotalSize = 0u;
    Vec2U levelSize = Vec2U(size.x, size.y);

    for (uint level = 0u; level < mipLevels; ++level)
    {
        bufferTotalSize += levelSize.x * levelSize.y * size.z * pixelSize;
        levelSize = maths::max(Vec2U(1u, 1u), levelSize / 2u);
    };

    return bufferTotalSize;
}

//==============================================================================

void Texture::add_to_bindless_descriptor_set(vk::DescriptorSet descriptorSet, uint32_t index)
{
    SQASSERT(mBindlessDescriptorIndex.has_value() == false, "already added to bindless descriptor set");

    const auto& ctx = VulkanContext::get();

    vk_update_descriptor_set(ctx, descriptorSet, DescriptorImageSampler(0u, index, get_descriptor_info()));
    mBindlessDescriptorIndex = index;
}
