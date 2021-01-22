#include <sqee/vk/Helpers.hpp>

using namespace sq;

//============================================================================//

StagingBuffer::StagingBuffer(const VulkanContext& _ctx, size_t size) : ctx(_ctx)
{
    buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, {}
        }
    );
    memory = ctx.allocator->allocate(ctx.device.getBufferMemoryRequirements(buffer), true);
    ctx.device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());
}

StagingBuffer::~StagingBuffer()
{
    ctx.device.destroy(buffer);
    memory.free();
}

//============================================================================//

OneTimeCommands::OneTimeCommands(const VulkanContext& _ctx) : ctx(_ctx)
{
    cmdbuf = ctx.device.allocateCommandBuffers (
        vk::CommandBufferAllocateInfo {
            ctx.commandPool, vk::CommandBufferLevel::ePrimary, 1u
        }
    ).front();

    cmdbuf.begin (
        vk::CommandBufferBeginInfo {
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr
        }
    );
}

OneTimeCommands::~OneTimeCommands()
{
    cmdbuf.end();

    ctx.queue.submit(vk::SubmitInfo({}, {}, cmdbuf, {}));
    ctx.queue.waitIdle();

    ctx.device.free(ctx.commandPool, cmdbuf);
}

//============================================================================//

std::tuple<vk::Buffer, VulkanMemory> sq::vk_create_buffer(const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host)
{
    auto buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, usage, vk::SharingMode::eExclusive, {}
        }
    );

    auto memory = ctx.allocator->allocate(ctx.device.getBufferMemoryRequirements(buffer), host);

    ctx.device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());

    return { buffer, memory };
}

//============================================================================//

std::tuple<vk::Image, VulkanMemory> sq::vk_create_image_2D(const VulkanContext& ctx, vk::Format format, Vec2U size, bool linear, vk::ImageUsageFlags usage, bool host)
{
    auto image = ctx.device.createImage (
        vk::ImageCreateInfo {
            {}, vk::ImageType::e2D, format, vk::Extent3D(size.x, size.y, 1u),
            1u, 1u, vk::SampleCountFlagBits::e1,
            linear ? vk::ImageTiling::eLinear : vk::ImageTiling::eOptimal, usage,
            vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined
        }
    );

    auto memory = ctx.allocator->allocate(ctx.device.getImageMemoryRequirements(image), host);

    ctx.device.bindImageMemory(image, memory.get_memory(), memory.get_offset());

    return { image, memory };
}

////============================================================================//

//void sq::vk_copy_buffer(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size)
//{
//    auto cmdbuf = vk_begin_one_time_commands(ctx);

//    cmdbuf.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy{0u, 0u, size});

//    vk_end_one_time_commands(ctx, cmdbuf);
//}

////============================================================================//

////void sq::vk_cmd_transition_image_layout(vk::CommandBuffer cmdbuf, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
////{
////    cmdbuf.pipelineBarrier (
////        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
////        {}, {}, {},
////        vk::ImageMemoryBarrier {
////            {}, vk::AccessFlagBits::eTransferWrite,
////            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
////            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
////            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
////        }
////    );
////}

////============================================================================//

//void sq::vk_transfer_buffer_to_image(const VulkContext& ctx, vk::Buffer srcBuffer, vk::Image dstImage, Vec2U size)
//{
//    auto cmdbuf = vk_begin_one_time_commands(ctx);

//    cmdbuf.pipelineBarrier (
//        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
//        {}, {}, {},
//        vk::ImageMemoryBarrier {
//            {}, vk::AccessFlagBits::eTransferWrite,
//            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
//            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dstImage,
//            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
//        }
//    );

//    cmdbuf.copyBufferToImage (
//        srcBuffer, dstImage, vk::ImageLayout::eTransferDstOptimal,
//        vk::BufferImageCopy {
//            0u, 0u, 0u,
//            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
//            vk::Offset3D(0u, 0u, 0u), vk::Extent3D(size.x, size.y, 1u)
//        }
//    );

//    cmdbuf.pipelineBarrier (
//        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
//        {}, {}, {},
//        vk::ImageMemoryBarrier {
//            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
//            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
//            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dstImage,
//            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
//        }
//    );

//    vk_end_one_time_commands(ctx, cmdbuf);
//}

////============================================================================//

//void sq::vk_copy_to_buffer(const VulkContext& ctx, void* data, vk::Buffer buffer, size_t size)
//{
//    auto stagingBuffer = ctx.device.createBuffer (
//        vk::BufferCreateInfo {
//            {}, size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, {}
//        }
//    );

//    auto stagingMem = ctx.allocator->allocate(ctx.device.getBufferMemoryRequirements(stagingBuffer), true);
//    ctx.device.bindBufferMemory(stagingBuffer, stagingMem.get_memory(), stagingMem.get_offset());

//    std::memcpy(stagingMem.map(), data, size);
//    stagingMem.unmap();

//    auto cmdbuf = vk_begin_one_time_commands(ctx);

//    cmdbuf.copyBuffer(stagingBuffer, buffer, vk::BufferCopy(0u, 0u, size));

//    vk_end_one_time_commands(ctx, cmdbuf);

//    ctx.device.destroy(stagingBuffer);
//    stagingMem.free();
//}

////============================================================================//

////void sq::vk_copy_to_image(const VulkContext& ctx, void* data, vk::Image image, Vec3U offset, Vec3U size)
////{
////    const size_t length = size_t(size.x * size.y * size.z);

////    auto stagingBuffer = ctx.device.createBuffer (
////        vk::BufferCreateInfo {
////            {}, length, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, {}
////        }
////    );

////    auto stagingMem = ctx.allocator->allocate(ctx.device.getBufferMemoryRequirements(stagingBuffer), true);
////    ctx.device.bindBufferMemory(stagingBuffer, stagingMem.get_memory(), stagingMem.get_offset());

////    std::memcpy(stagingMem.map(), data, length);
////    stagingMem.unmap();

////    auto cmdbuf = vk_begin_one_time_commands(ctx);

////    cmdbuf.pipelineBarrier (
////        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
////        {}, {}, {},
////        vk::ImageMemoryBarrier {
////            {}, vk::AccessFlagBits::eTransferWrite,
////            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
////            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
////            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
////        }
////    );

////    cmdbuf.copyBufferToImage (
////        stagingBuffer, image, vk::ImageLayout::eTransferDstOptimal,
////        vk::BufferImageCopy {
////            0u, 0u, 0u,
////            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
////            offset, size
////        }
////    );

////    cmdbuf.pipelineBarrier (
////        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
////        {}, {}, {},
////        vk::ImageMemoryBarrier {
////            vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
////            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
////            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dstImage,
////            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
////        }
////    );

////    vk_end_one_time_commands(ctx, cmdbuf);
////}
