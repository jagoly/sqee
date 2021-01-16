#pragma once

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

struct VulkContext
{
    vk::Device device;
    vk::CommandPool commandPool;
    vk::DescriptorPool descriptorPool;
    vk::Queue queue;
    vk::CommandBuffer commandBuffer;
    vk::RenderPass renderPass;
    vk::Framebuffer framebuffer;

    Vec2U windowSize;

    uint memoryTypeHost;
    uint memoryTypeDevice;
    float maxAnisotropy;
};

} // namespace sq
