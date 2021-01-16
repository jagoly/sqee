#include <sqee/vk/Helpers.hpp>

#include <sqee/vk/VulkWindow.hpp>

using namespace sq;

//============================================================================//

inline vk::CommandBuffer vk_begin_one_time_commands(const VulkContext& ctx)
{
    auto cmdbuf = ctx.device.allocateCommandBuffers (
        vk::CommandBufferAllocateInfo {
            ctx.commandPool, vk::CommandBufferLevel::ePrimary, 1u
        }
    ).front();

    cmdbuf.begin (
        vk::CommandBufferBeginInfo {
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr
        }
    );

    return cmdbuf;
}

inline void vk_end_one_time_commands(const VulkContext& ctx, vk::CommandBuffer cmdbuf)
{
    cmdbuf.end();

    ctx.queue.submit(vk::SubmitInfo({}, {}, cmdbuf, {}));
    ctx.queue.waitIdle();

    ctx.device.free(ctx.commandPool, cmdbuf);
}

//============================================================================//

std::tuple<vk::Buffer, vk::DeviceMemory> sq::vk_create_buffer(const VulkContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host)
{
    auto buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, usage, vk::SharingMode::eExclusive, {}
        }
    );

    auto bufferMem = ctx.device.allocateMemory (
        vk::MemoryAllocateInfo {
            ctx.device.getBufferMemoryRequirements(buffer).size,
            host ? ctx.memoryTypeHost : ctx.memoryTypeDevice
        }
    );

    ctx.device.bindBufferMemory(buffer, bufferMem, 0u);

    return { buffer, bufferMem };
}

//============================================================================//

std::tuple<vk::Image, vk::DeviceMemory> sq::vk_create_image(const VulkContext& ctx, Vec2U size, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, bool host)
{
    auto image = ctx.device.createImage (
        vk::ImageCreateInfo {
            {}, vk::ImageType::e2D, format, {size.x, size.y, 1u},
            1u, 1u, vk::SampleCountFlagBits::e1, tiling, usage,
            vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined
        }
    );

    auto imageMem = ctx.device.allocateMemory (
        vk::MemoryAllocateInfo {
            ctx.device.getImageMemoryRequirements(image).size,
            host ? ctx.memoryTypeHost : ctx.memoryTypeDevice
        }
    );

    ctx.device.bindImageMemory(image, imageMem, 0u);

    return { image, imageMem };
}

//============================================================================//

void sq::vk_copy_buffer(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size)
{
    auto cmdbuf = vk_begin_one_time_commands(ctx);

    cmdbuf.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy{0u, 0u, size});

    vk_end_one_time_commands(ctx, cmdbuf);
}

//============================================================================//

void sq::vk_transfer_buffer_to_image(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Image dstImage, Vec2U size)
{
    auto cmdbuf = vk_begin_one_time_commands(ctx);

    cmdbuf.pipelineBarrier (
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            {}, vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dstImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
        }
    );

    cmdbuf.copyBufferToImage (
        srcBuffer, dstImage, vk::ImageLayout::eTransferDstOptimal,
        vk::BufferImageCopy {
            0u, 0u, 0u,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
            vk::Offset3D(0u, 0u, 0u), vk::Extent3D(size.x, size.y, 1u)
        }
    );

    cmdbuf.pipelineBarrier (
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
        {}, {}, {},
        vk::ImageMemoryBarrier {
            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dstImage,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
        }
    );

    vk_end_one_time_commands(ctx, cmdbuf);
}
