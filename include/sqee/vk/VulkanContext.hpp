#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

class VulkanAllocator;

struct VulkanContext final : NonCopyable
{
    struct Limits
    {
        float maxAnisotropy;
    };

    struct Frame
    {
        vk::CommandBuffer commandBuffer;
        vk::Framebuffer framebuffer;
    };

    vk::Device device;
    vk::Queue queue;
    vk::CommandPool commandPool;
    vk::DescriptorPool descriptorPool;
    vk::RenderPass renderPass;

    VulkanAllocator* allocator;

    Vec2U windowSize;

    /// Relevant hardware limits from device.
    Limits limits;

    /// Objects that get swapped each frame.
    Frame frame;
};

//============================================================================//

} // namespace sq
