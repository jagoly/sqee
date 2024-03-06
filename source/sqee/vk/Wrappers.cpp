#include <sqee/vk/Wrappers.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/vk/VulkanContext.hpp>

using namespace sq;

//============================================================================//

void BufferStuff::initialise(const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host)
{
    buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo (
            {}, size, usage, vk::SharingMode::eExclusive, nullptr
        )
    );

    memory = ctx.allocator.allocate(ctx.device.getBufferMemoryRequirements(buffer), host);
    ctx.device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());
}

//============================================================================//

void ImageStuff::initialise_2D(const VulkanContext& ctx, vk::Format format, Vec2U size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect)
{
    image = ctx.device.createImage (
        vk::ImageCreateInfo (
            {}, vk::ImageType::e2D, format, vk::Extent3D(size.x, size.y, 1u), mipLevels, 1u, samples,
            vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, nullptr, vk::ImageLayout::eUndefined
        )
    );

    memory = ctx.allocator.allocate(ctx.device.getImageMemoryRequirements(image), host);
    ctx.device.bindImageMemory(image, memory.get_memory(), memory.get_offset());

    view = ctx.device.createImageView (
        vk::ImageViewCreateInfo (
            {}, image, vk::ImageViewType::e2D, format, swizzle, vk::ImageSubresourceRange(aspect, 0u, mipLevels, 0u, 1u)
        )
    );
}

//============================================================================//

void ImageStuff::initialise_array(const VulkanContext& ctx, vk::Format format, Vec3U size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect)
{
    image = ctx.device.createImage (
        vk::ImageCreateInfo (
            {}, vk::ImageType::e2D, format, vk::Extent3D(size.x, size.y, 1u), mipLevels, size.z, samples,
            vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, nullptr, vk::ImageLayout::eUndefined
        )
    );

    memory = ctx.allocator.allocate(ctx.device.getImageMemoryRequirements(image), host);
    ctx.device.bindImageMemory(image, memory.get_memory(), memory.get_offset());

    view = ctx.device.createImageView (
        vk::ImageViewCreateInfo (
            {}, image, vk::ImageViewType::e2DArray, format, swizzle, vk::ImageSubresourceRange(aspect, 0u, mipLevels, 0u, size.z)
        )
    );
}

//============================================================================//

void ImageStuff::initialise_cube(const VulkanContext& ctx, vk::Format format, uint size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect)
{
    image = ctx.device.createImage (
        vk::ImageCreateInfo (
            vk::ImageCreateFlagBits::eCubeCompatible, vk::ImageType::e2D, format, vk::Extent3D(size, size, 1u), mipLevels, 6u, samples,
            vk::ImageTiling::eOptimal, usage, vk::SharingMode::eExclusive, nullptr, vk::ImageLayout::eUndefined
        )
    );

    memory = ctx.allocator.allocate(ctx.device.getImageMemoryRequirements(image), host);
    ctx.device.bindImageMemory(image, memory.get_memory(), memory.get_offset());

    view = ctx.device.createImageView (
        vk::ImageViewCreateInfo (
            {}, image, vk::ImageViewType::eCube, format, swizzle, vk::ImageSubresourceRange(aspect, 0u, mipLevels, 0u, 6u)
        )
    );
}

//============================================================================//

void RenderPassStuff::initialise(const VulkanContext& ctx, ArrayProxyRef<vk::AttachmentDescription> attachments, ArrayProxyRef<vk::SubpassDescription> subpasses, ArrayProxyRef<vk::SubpassDependency> dependencies, Vec2U imageSize, uint layers, ArrayProxyRef<vk::ImageView> imageViews)
{
    pass = ctx.device.createRenderPass (
        vk::RenderPassCreateInfo (
            {}, attachments.size(), attachments.data(), subpasses.size(), subpasses.data(), dependencies.size(), dependencies.data()
        )
    );

    framebuf = ctx.device.createFramebuffer (
        vk::FramebufferCreateInfo (
            {}, pass, imageViews.size(), imageViews.data(), imageSize.x, imageSize.y, layers
        )
    );
}

//============================================================================//

void BufferStuff::destroy(const VulkanContext& ctx)
{
    ctx.device.destroy(buffer);
    memory.free();
}

void ImageStuff::destroy(const VulkanContext& ctx)
{
    ctx.device.destroy(image);
    ctx.device.destroy(view);
    memory.free();
}

void RenderPassStuff::destroy(const VulkanContext& ctx)
{
    ctx.device.destroy(pass);
    ctx.device.destroy(framebuf);
}
