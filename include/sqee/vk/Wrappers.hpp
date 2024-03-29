#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq { //###############################################################

class VulkanContext;

template <class T>
using ArrayProxyRef = const vk::ArrayProxy<const T>&;

//==============================================================================

struct SQEE_API BufferStuff
{
    vk::Buffer buffer;
    VulkanMemory memory;

    void initialise (
        const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host
    );

    void destroy(const VulkanContext& ctx);
};

//==============================================================================

struct SQEE_API ImageStuff
{
    vk::Image image;
    VulkanMemory memory;
    vk::ImageView view;

    void initialise_2D (
        const VulkanContext& ctx, vk::Format format, Vec2U size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
    );

    void initialise_array (
        const VulkanContext& ctx, vk::Format format, Vec3U size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
    );

    void initialise_cube (
        const VulkanContext& ctx, vk::Format format, uint size, uint mipLevels, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
    );

    void destroy(const VulkanContext& ctx);
};

//==============================================================================

struct SQEE_API RenderPassStuff
{
    vk::RenderPass pass;
    vk::Framebuffer framebuf;

    void initialise (
        const VulkanContext& ctx, ArrayProxyRef<vk::AttachmentDescription> attachments, ArrayProxyRef<vk::SubpassDescription> subpasses, ArrayProxyRef<vk::SubpassDependency> dependencies, Vec2U imageSize, uint layers, ArrayProxyRef<vk::ImageView> imageViews
    );

    void destroy(const VulkanContext& ctx);
};

} // namespace sq ##############################################################
