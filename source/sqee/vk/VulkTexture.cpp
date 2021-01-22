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

static vk::SamplerAddressMode impl_string_to_wrap(const String& arg)
{
    if (arg == "Repeat") return vk::SamplerAddressMode::eRepeat;
    if (arg == "MirrorRepeat") return vk::SamplerAddressMode::eMirroredRepeat;
    if (arg == "Clamp") return vk::SamplerAddressMode::eClampToEdge;
    if (arg == "MirrorClamp") return vk::SamplerAddressMode::eMirrorClampToEdge;

    log_error("invalid texture wrap string '{}'", arg);
}

//============================================================================//

inline int impl_get_format_channels(vk::Format format)
{
    if (format == vk::Format::eR8Srgb) return 1;
    if (format == vk::Format::eR8G8Srgb) return 2;
    if (format == vk::Format::eR8G8B8A8Srgb) return 4;

    if (format == vk::Format::eR8Unorm) return 1;
    if (format == vk::Format::eR8G8Unorm) return 2;
    if (format == vk::Format::eR8G8B8A8Unorm) return 4;

    if (format == vk::Format::eR8Snorm) return 1;
    if (format == vk::Format::eR8G8Snorm) return 2;
    if (format == vk::Format::eR8G8B8A8Snorm) return 4;

    return 0;
}

inline bool impl_get_format_signed(vk::Format format)
{
    if (format == vk::Format::eR8Snorm) return true;
    if (format == vk::Format::eR8G8Snorm) return true;
    if (format == vk::Format::eR8G8B8A8Snorm) return true;

    return false;
}

//============================================================================//

static auto impl_load_image(vk::Format format, const String& path)
{
    struct Image
    {
        ~Image() { stbi_image_free(data); };
        uint8_t* data; Vec2U size; size_t length;
    };

    const int formatChannels = impl_get_format_channels(format);

    int width, height, channels;
    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, formatChannels);

    if (data == nullptr)
        log_error_multiline("Error loading image '{}':\n{}", path, stbi_failure_reason());

    if (impl_get_format_signed(format) == true)
    {
        int8_t* begin = reinterpret_cast<int8_t*>(data);
        int8_t* end = begin + width*height*channels;

        for (int8_t* p = begin; p != end; ++p)
            *p += *p >= 0 ? -128 : +128;
    }

    return Image { data, Vec2U(width, height), size_t(width * height * formatChannels) };
}

//============================================================================//

VulkTexture::VulkTexture(VulkTexture&& other)
{
    *this = std::move(other);
}

VulkTexture& VulkTexture::operator=(VulkTexture&& other)
{
    std::swap(ctx, other.ctx);
    std::swap(mImage, other.mImage);
    std::swap(mImageMem, other.mImageMem);
    std::swap(mImageView, other.mImageView);
    std::swap(mSampler, other.mSampler);
    return *this;
}

VulkTexture::~VulkTexture()
{
    if (ctx == nullptr) return;
    ctx->device.destroy(mSampler);
    ctx->device.destroy(mImageView);
    ctx->device.destroy(mImage);
    mImageMem.free();
}

//============================================================================//

void VulkTexture::load_automatic_2D(const VulkanContext& _ctx, const String& path)
{
    SQASSERT(ctx == nullptr, "texture already loaded");
    ctx = &_ctx;

    const JsonValue json = parse_json_from_file(path + ".json");

    const auto format = impl_string_to_format(json.at("format"));
    //const bool mipmaps = json.at("mipmaps");

    const auto image = impl_load_image(format, path + ".png");

    // create image and allocate memory
    {
        mImage = ctx->device.createImage (
            vk::ImageCreateInfo {
                {}, vk::ImageType::e2D, format, {image.size.x, image.size.y, 1u},
                1u, 1u, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined
            }
        );

        mImageMem = ctx->allocator->allocate(ctx->device.getImageMemoryRequirements(mImage), false);
        ctx->device.bindImageMemory(mImage, mImageMem.get_memory(), mImageMem.get_offset());
    }

    // load image data via staging buffer
    {
        auto staging = StagingBuffer(*ctx, image.length);
        auto cmdbuf = OneTimeCommands(*ctx);

        std::memcpy(staging.memory.map(), image.data, image.length);
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
                vk::Offset3D(0u, 0u, 0u), vk::Extent3D(image.size.x, image.size.y, 1u)
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

    // create image view
    {
        const auto components = impl_string_to_swizzle(json.at("swizzle"));

        mImageView = ctx->device.createImageView (
            vk::ImageViewCreateInfo {
                {}, mImage, vk::ImageViewType::e2D, format, components,
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
            }
        );
    }

    // create sampler
    {
        const bool filterBool = json.at("filter");

        const auto magFilter = filterBool ? vk::Filter::eLinear : vk::Filter::eNearest;
        const auto minFilter = filterBool ? vk::Filter::eLinear : vk::Filter::eNearest;
        const auto mipmapMode = filterBool ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest;

        const auto addressModeU = impl_string_to_wrap(json.at("wrapX"));
        const auto addressModeV = impl_string_to_wrap(json.at("wrapY"));

        mSampler = ctx->device.createSampler (
            vk::SamplerCreateInfo {
                {}, magFilter, minFilter, mipmapMode,
                addressModeU, addressModeV, vk::SamplerAddressMode::eRepeat,
                0.f, filterBool, filterBool ? ctx->limits.maxAnisotropy : 0.f,
                false, vk::CompareOp::eAlways, 0.f, 0.f,
                vk::BorderColor::eFloatTransparentBlack, false
            }
        );
    }

    mDescriptorInfo = vk::DescriptorImageInfo(mSampler, mImageView, vk::ImageLayout::eShaderReadOnlyOptimal);
}
