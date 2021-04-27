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
    if (arg == "1x8_Srgb") return vk::Format::eR8Srgb;
    if (arg == "2x8_Srgb") return vk::Format::eR8G8Srgb;
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

    const auto& ctx = sq::VulkanContext::get();

    std::tie(mImage, mImageMem, mImageView) = vk_create_image_2D (
        ctx, config.format, Vec2U(config.size), vk::SampleCountFlagBits::e1,
        false, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_common(ctx, config);
}

//============================================================================//

void VulkTexture::load_from_memory_2D(void* data, const Config& config)
{
    const auto& ctx = sq::VulkanContext::get();

    const auto formatInfo = impl_get_format_info(config.format);

    auto staging = StagingBuffer(ctx, config.size.x * config.size.y * formatInfo.pixelSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, config.size.x * config.size.y * formatInfo.pixelSize);
    staging.memory.unmap();

    cmdbuf->pipelineBarrier (
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            {}, vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
        }
    );

    cmdbuf->copyBufferToImage (
        staging.buffer, mImage, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy {
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
            vk::Offset3D(0u, 0u, 0u), vk::Extent3D(config.size.x, config.size.y, 1u)
        }
    );

    cmdbuf->pipelineBarrier (
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllGraphics,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
        }
    );
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
    config.mipmaps = json.at("mipmaps");
    config.anisotropy = json.at("anisotropy");

    const auto image = impl_load_image(config.format, path + ".png");
    config.size = Vec3U(image.size, 1u);

    initialise_2D(config);
    load_from_memory_2D(image.data, config);
}

//============================================================================//

void VulkTexture::initialise_cube(const Config& config)
{
    SQASSERT(!mImage, "texture already loaded");

    const auto& ctx = sq::VulkanContext::get();

    std::tie(mImage, mImageMem, mImageView) = vk_create_image_cube (
        ctx, config.format, config.size.x, vk::SampleCountFlagBits::e1,
        false, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        false, config.swizzle, vk::ImageAspectFlagBits::eColor
    );

    impl_initialise_common(ctx, config);
}

//============================================================================//

void VulkTexture::load_from_memory_cube(void* data, uint face, const Config& config)
{
    const auto& ctx = sq::VulkanContext::get();

    const auto formatInfo = impl_get_format_info(config.format);

    auto staging = StagingBuffer(ctx, config.size.x * config.size.y * formatInfo.pixelSize);
    auto cmdbuf = OneTimeCommands(ctx);

    std::memcpy(staging.memory.map(), data, config.size.x * config.size.y * formatInfo.pixelSize);
    staging.memory.unmap();

    cmdbuf->pipelineBarrier (
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            {}, vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, face, 1u)
        }
    );

    cmdbuf->copyBufferToImage (
        staging.buffer, mImage, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy {
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, face, 1u),
            vk::Offset3D(0u, 0u, 0u), vk::Extent3D(config.size.x, config.size.y, 1u)
        }
    );

    cmdbuf->pipelineBarrier (
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllGraphics,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, mImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, face, 1u)
        }
    );
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
    config.mipmaps = json.at("mipmaps");
    config.anisotropy = json.at("anisotropy");
    config.size = Vec3U(Vec2U(uint(json.at("size"))), 6u);

    initialise_cube(config);

    for (const char* fname : {"0_right", "1_left", "2_forward", "3_back", "4_up", "5_down"})
    {
        const auto image = impl_load_image(config.format, fmt::format("{}/{}.png", path, fname));
        SQASSERT(image.size == Vec2U(config.size), "image size does not match json");
        load_from_memory_cube(image.data, uint(*fname - '0'), config);
    }
}

//============================================================================//

void VulkTexture::impl_initialise_common(const VulkanContext& ctx, const Config& config)
{
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
