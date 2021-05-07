#pragma once

#include <sqee/export.hpp>

#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

template <class T>
using ArrayProxyRef = const vk::ArrayProxy<const T>&;

//============================================================================//

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

struct SQEE_API ShaderModules
{
    using ShaderData = std::pair<const uint32_t*, size_t>;
    ShaderModules(const VulkanContext& ctx, const String& vertex, const String& geometry, const String& fragment);
    ShaderModules(const VulkanContext& ctx, ShaderData vertex, ShaderData geometry, ShaderData fragment);
    ~ShaderModules();
    const VulkanContext& ctx;
    std::vector<vk::ShaderModule> modules;
    std::vector<vk::PipelineShaderStageCreateInfo> stages;
};

//============================================================================//

SQEE_API std::tuple<vk::Buffer, VulkanMemory> vk_create_buffer (
    const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host
);

SQEE_API std::tuple<vk::Image, VulkanMemory, vk::ImageView> vk_create_image_2D (
    const VulkanContext& ctx, vk::Format format, Vec2U size, uint mipLevels, vk::SampleCountFlagBits samples, bool linear, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
);

SQEE_API std::tuple<vk::Image, VulkanMemory, vk::ImageView> vk_create_image_array (
    const VulkanContext& ctx, vk::Format format, Vec3U size, vk::SampleCountFlagBits samples, bool linear, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
);

SQEE_API std::tuple<vk::Image, VulkanMemory, vk::ImageView> vk_create_image_cube (
    const VulkanContext& ctx, vk::Format format, uint size, vk::SampleCountFlagBits samples, bool linear, vk::ImageUsageFlags usage, bool host, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect
);

//============================================================================//

SQEE_API vk::Pipeline vk_create_graphics_pipeline (
    const VulkanContext& ctx, vk::PipelineLayout layout, vk::RenderPass renderPass, uint32_t subpass,
    ArrayProxyRef<vk::PipelineShaderStageCreateInfo> stages,
    const vk::PipelineVertexInputStateCreateInfo& vertexInputState,
    const vk::PipelineInputAssemblyStateCreateInfo& inputAssemblyState,
    const vk::PipelineRasterizationStateCreateInfo& rasterizationState,
    const vk::PipelineMultisampleStateCreateInfo& multisampleState,
    const vk::PipelineDepthStencilStateCreateInfo& depthStencilState,
    ArrayProxyRef<vk::Viewport> viewports,
    ArrayProxyRef<vk::Rect2D> scissors,
    ArrayProxyRef<vk::PipelineColorBlendAttachmentState> colorBlendAttachments,
    ArrayProxyRef<vk::DynamicState> dynamicStates
);

// todo: don't want to try to wrap this until I know what I'm doing
//SQEE_API vk::RenderPass vk_create_render_pass ();

//============================================================================//

inline vk::DescriptorSetLayout vk_create_descriptor_set_layout (
    const VulkanContext& ctx, vk::DescriptorSetLayoutCreateFlags flags, ArrayProxyRef<vk::DescriptorSetLayoutBinding> bindings
) {
    return ctx.device.createDescriptorSetLayout (
        vk::DescriptorSetLayoutCreateInfo {
            flags, bindings.size(), bindings.data()
        }
    );
}

inline vk::PipelineLayout vk_create_pipeline_layout (
    const VulkanContext& ctx, vk::PipelineLayoutCreateFlags flags, ArrayProxyRef<vk::DescriptorSetLayout> setLayouts, ArrayProxyRef<vk::PushConstantRange> pushConstantRanges
) {
    return ctx.device.createPipelineLayout (
        vk::PipelineLayoutCreateInfo {
            flags, setLayouts.size(), setLayouts.data(), pushConstantRanges.size(), pushConstantRanges.data()
        }
    );
}

//============================================================================//

SQEE_API vk::DescriptorSet vk_allocate_descriptor_set (
    const VulkanContext& ctx, vk::DescriptorSetLayout layout
);

SQEE_API Swapper<vk::DescriptorSet> vk_allocate_descriptor_set_swapper (
    const VulkanContext& ctx, vk::DescriptorSetLayout layout
);

//============================================================================//

inline void vk_update_descriptor_set (
    const VulkanContext& ctx, vk::DescriptorSet& dset, uint binding, uint index, vk::DescriptorType type, ArrayProxyRef<vk::DescriptorBufferInfo> bufferInfo
) {
    ctx.device.updateDescriptorSets (
        vk::WriteDescriptorSet {
            dset, binding, index, bufferInfo.size(), type, nullptr, bufferInfo.data(), nullptr
        }, {}
    );
}

inline void vk_update_descriptor_set_swapper (
    const VulkanContext& ctx, Swapper<vk::DescriptorSet>& swapper, uint binding, uint index, vk::DescriptorType type, ArrayProxyRef<vk::DescriptorBufferInfo> bufferInfoFront, ArrayProxyRef<vk::DescriptorBufferInfo> bufferInfoBack
) {
    ctx.device.updateDescriptorSets (
        {
            vk::WriteDescriptorSet {
                swapper.front, binding, index, bufferInfoFront.size(), type, nullptr, bufferInfoFront.data(), nullptr
            },
            vk::WriteDescriptorSet {
                swapper.back, binding, index, bufferInfoBack.size(), type, nullptr, bufferInfoBack.data(), nullptr
            },
        }, {}
    );
}

//============================================================================//

inline void vk_update_descriptor_set (
    const VulkanContext& ctx, vk::DescriptorSet& dset, uint binding, uint index, vk::DescriptorType type, ArrayProxyRef<vk::DescriptorImageInfo> imageInfo
) {
    ctx.device.updateDescriptorSets (
        vk::WriteDescriptorSet {
            dset, binding, index, uint(imageInfo.size()), type, imageInfo.data(), nullptr, nullptr
        }, {}
    );
}

inline void vk_update_descriptor_set_swapper (
    const VulkanContext& ctx, Swapper<vk::DescriptorSet>& swapper, uint binding, uint index, vk::DescriptorType type, ArrayProxyRef<vk::DescriptorImageInfo> imageInfoFront, ArrayProxyRef<vk::DescriptorImageInfo> imageInfoBack
) {
    ctx.device.updateDescriptorSets (
        {
            vk::WriteDescriptorSet {
                swapper.front, binding, index, uint(imageInfoFront.size()), type, imageInfoFront.data(), nullptr, nullptr
            },
            vk::WriteDescriptorSet {
                swapper.back, binding, index, uint(imageInfoBack.size()), type, imageInfoBack.data(), nullptr, nullptr
            },
        }, {}
    );
}

//============================================================================//

inline void vk_transition_image_memory_layout (
    vk::CommandBuffer cmdbuf, vk::Image image, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectMask, uint baseMipLevel, uint levelCount, uint baseArrayLayer, uint layerCount
) {
    cmdbuf.pipelineBarrier (
        srcStageMask, dstStageMask, {}, {}, {},
        vk::ImageMemoryBarrier {
            srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
            vk::ImageSubresourceRange(aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount)
        }
    );
}

//============================================================================//

} // namespace sq
