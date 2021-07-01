#pragma once

#include <sqee/export.hpp>

#include <sqee/core/Utilities.hpp>

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
    StagingBuffer(const VulkanContext& ctx, bool source, bool destination, size_t size);
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
    ShaderModules(const VulkanContext& ctx, const String& vertex, const String& geometry, const String& fragment, const vk::SpecializationInfo* specialisation = nullptr);
    ShaderModules(const VulkanContext& ctx, ShaderData vertex, ShaderData geometry, ShaderData fragment, const vk::SpecializationInfo* specialisation = nullptr);
    ~ShaderModules();
    const VulkanContext& ctx;
    std::vector<vk::ShaderModule> modules;
    std::vector<vk::PipelineShaderStageCreateInfo> stages;
};

//============================================================================//

template <class... Values> struct SpecialisationInfo
{
    static_assert (
        ((std::is_same_v<int, Values> || std::is_same_v<float, Values>) && ...),
        "specialisation constants can only be ints or floats"
    );

    template <size_t... Indices>
    SpecialisationInfo(Values... values, std::index_sequence<Indices...>)
        : data { values... }
        , map { vk::SpecializationMapEntry(Indices, Indices * 4u, 4u)... }
        , info ( sizeof...(Values), map.data(), sizeof(data), &data ) {}

    SpecialisationInfo(Values... values)
        : SpecialisationInfo(values..., std::index_sequence_for<Values...>()) {}

    Structure<Values...> data;
    std::array<vk::SpecializationMapEntry, sizeof...(Values)> map;
    vk::SpecializationInfo info;
};

template<class... Values> SpecialisationInfo(Values...) -> SpecialisationInfo<Values...>;

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

//============================================================================//

SQEE_API vk::DescriptorSet vk_allocate_descriptor_set (
    const VulkanContext& ctx, vk::DescriptorSetLayout layout
);

SQEE_API Swapper<vk::DescriptorSet> vk_allocate_descriptor_set_swapper (
    const VulkanContext& ctx, vk::DescriptorSetLayout layout
);

//============================================================================//

struct DescriptorUniformBuffer
{
    DescriptorUniformBuffer(uint _binding, uint _index, vk::DescriptorBufferInfo info)
        : binding(_binding), index(_index), front(info), back(info) {}

    DescriptorUniformBuffer(uint _binding, uint _index, vk::Buffer buffer, size_t offset, size_t range)
        : binding(_binding), index(_index), front(buffer, offset, range), back(buffer, offset, range) {}

    DescriptorUniformBuffer(uint _binding, uint _index, Swapper<vk::DescriptorBufferInfo> info)
        : binding(_binding), index(_index), front(info.front), back(info.back) {}

    uint binding, index;
    vk::DescriptorBufferInfo front, back;
};

struct DescriptorImageSampler
{
    DescriptorImageSampler(uint _binding, uint _index, vk::DescriptorImageInfo info)
        : binding(_binding), index(_index), front(info), back(info) {}

    DescriptorImageSampler(uint _binding, uint _index, vk::Sampler sampler, vk::ImageView view, vk::ImageLayout layout)
        : binding(_binding), index(_index), front(sampler, view, layout), back(sampler, view, layout) {}

    DescriptorImageSampler(uint _binding, uint _index, Swapper<vk::DescriptorImageInfo> info)
        : binding(_binding), index(_index), front(info.front), back(info.back) {}

    uint binding, index;
    vk::DescriptorImageInfo front, back;
};

//============================================================================//

namespace detail {

inline vk::WriteDescriptorSet make_write_descriptor_set(vk::DescriptorSet dset, const DescriptorUniformBuffer& descriptor, bool back)
{
    return vk::WriteDescriptorSet {
        dset, descriptor.binding, descriptor.index, 1u, vk::DescriptorType::eUniformBuffer,
        nullptr, back ? &descriptor.back : &descriptor.front, nullptr
    };
}

inline vk::WriteDescriptorSet make_write_descriptor_set(vk::DescriptorSet dset, const DescriptorImageSampler& descriptor, bool back)
{
    return vk::WriteDescriptorSet {
        dset, descriptor.binding, descriptor.index, 1u, vk::DescriptorType::eCombinedImageSampler,
        back ? &descriptor.back : &descriptor.front, nullptr, nullptr
    };
}

} // namespace detail

//============================================================================//

template <class... Descriptors>
inline void vk_update_descriptor_set(const VulkanContext& ctx, vk::DescriptorSet dset, Descriptors... descriptors)
{
    const auto writes = std::array {
        detail::make_write_descriptor_set(dset, descriptors, false)...
    };

    ctx.device.updateDescriptorSets (
        uint(writes.size()), writes.data(), 0u, nullptr
    );
}

template <class... Descriptors>
inline void vk_update_descriptor_set_swapper(const VulkanContext& ctx, Swapper<vk::DescriptorSet> swapper, Descriptors... descriptors)
{
    const auto writes = std::array {
        detail::make_write_descriptor_set(swapper.front, descriptors, false)...,
        detail::make_write_descriptor_set(swapper.back, descriptors, true)...
    };

    ctx.device.updateDescriptorSets (
        uint(writes.size()), writes.data(), 0u, nullptr
    );
}

//============================================================================//

inline void vk_pipeline_barrier_image_memory (
    vk::CommandBuffer cmdbuf, vk::Image image, vk::DependencyFlags dependencyFlags, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectMask, uint baseMipLevel, uint levelCount, uint baseArrayLayer, uint layerCount
) {
    cmdbuf.pipelineBarrier (
        srcStageMask, dstStageMask, dependencyFlags, {}, {},
        vk::ImageMemoryBarrier {
            srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
            vk::ImageSubresourceRange(aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount)
        }
    );
}

//============================================================================//

} // namespace sq
