#pragma once

#include <sqee/export.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

struct SQEE_API StagingBuffer
{
    StagingBuffer(const VulkanContext& ctx, size_t size);
    ~StagingBuffer();
    const VulkanContext& ctx;
    vk::Buffer buffer;
    VulkanMemory memory;
};

struct SQEE_API OneTimeCommands
{
    OneTimeCommands(const VulkanContext& ctx);
    ~OneTimeCommands();
    auto operator->() { return &cmdbuf; }
    const VulkanContext& ctx;
    vk::CommandBuffer cmdbuf;
};

SQEE_API std::tuple<vk::Buffer, VulkanMemory> vk_create_buffer (
    const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host
);

SQEE_API std::tuple<vk::Image, VulkanMemory> vk_create_image_2D (
    const VulkanContext& ctx, vk::Format format, Vec2U size, bool linear, vk::ImageUsageFlags usage, bool host
);

} // namespace sq
