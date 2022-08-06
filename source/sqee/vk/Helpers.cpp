#include <sqee/vk/Helpers.hpp>

#include <sqee/misc/Files.hpp>

using namespace sq;

//============================================================================//

StagingBuffer::StagingBuffer(const VulkanContext& _ctx, bool source, bool destination, size_t size)
    : ctx(_ctx)
{
    vk::BufferUsageFlags usageFlags;
    if (source) usageFlags |= vk::BufferUsageFlagBits::eTransferSrc;
    if (destination) usageFlags |= vk::BufferUsageFlagBits::eTransferDst;

    buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, usageFlags, vk::SharingMode::eExclusive, {}
        }
    );
    memory = ctx.allocator.allocate(ctx.device.getBufferMemoryRequirements(buffer), true);
    ctx.device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());
}

StagingBuffer::~StagingBuffer()
{
    ctx.device.destroy(buffer);
    memory.free();
}

//============================================================================//

OneTimeCommands::OneTimeCommands(const VulkanContext& _ctx)
    : ctx(_ctx)
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

ShaderModules::ShaderModules(const VulkanContext& _ctx, const String& vertex, const String& geometry, const String& fragment, const vk::SpecializationInfo* specialisation)
    : ctx(_ctx)
{
    modules.reserve(size_t(!vertex.empty() + !geometry.empty() + !fragment.empty()));
    stages.reserve(size_t(!vertex.empty() + !geometry.empty() + !fragment.empty()));

    const auto load_shader = [&](const String& source, vk::ShaderStageFlagBits stage)
    {
        auto shaderCode = sq::read_bytes_from_file(source);

        auto shaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo { {}, shaderCode.size(), reinterpret_cast<uint32_t*>(shaderCode.data()) }
        );

        auto shaderStage = vk::PipelineShaderStageCreateInfo { {}, stage, shaderModule, "main", specialisation };

        modules.push_back(shaderModule);
        stages.push_back(shaderStage);
    };

    if (vertex.empty() == false) load_shader(vertex, vk::ShaderStageFlagBits::eVertex);
    if (geometry.empty() == false) load_shader(geometry, vk::ShaderStageFlagBits::eGeometry);
    if (fragment.empty() == false) load_shader(fragment, vk::ShaderStageFlagBits::eFragment);
}

ShaderModules::ShaderModules(const VulkanContext& _ctx, ShaderData vertex, ShaderData geometry, ShaderData fragment, const vk::SpecializationInfo* specialisation)
    : ctx(_ctx)
{
    modules.reserve(size_t(bool(vertex.first) + bool(geometry.first) + bool(fragment.first)));
    stages.reserve(size_t(bool(vertex.first) + bool(geometry.first) + bool(fragment.first)));

    const auto load_shader = [&](const ShaderData& source, vk::ShaderStageFlagBits stage)
    {
        auto shaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo { {}, std::get<1>(source), std::get<0>(source) }
        );

        auto shaderStage = vk::PipelineShaderStageCreateInfo { {}, stage, shaderModule, "main", specialisation };

        modules.push_back(shaderModule);
        stages.push_back(shaderStage);
    };

    if (vertex.first != nullptr) load_shader(vertex, vk::ShaderStageFlagBits::eVertex);
    if (geometry.first != nullptr) load_shader(geometry, vk::ShaderStageFlagBits::eGeometry);
    if (fragment.first != nullptr) load_shader(fragment, vk::ShaderStageFlagBits::eFragment);
}

ShaderModules::~ShaderModules()
{
    for (auto& module : modules)
        ctx.device.destroy(module);
}

//============================================================================//

vk::Pipeline sq::vk_create_graphics_pipeline(const VulkanContext& ctx, vk::PipelineLayout layout, vk::RenderPass renderPass, uint32_t subpass, ArrayProxyRef<vk::PipelineShaderStageCreateInfo> stages, const vk::PipelineVertexInputStateCreateInfo& vertexInputState, const vk::PipelineInputAssemblyStateCreateInfo& inputAssemblyState, const vk::PipelineRasterizationStateCreateInfo& rasterizationState, const vk::PipelineMultisampleStateCreateInfo& multisampleState, const vk::PipelineDepthStencilStateCreateInfo& depthStencilState, ArrayProxyRef<vk::Viewport> viewports, ArrayProxyRef<vk::Rect2D> scissors, ArrayProxyRef<vk::PipelineColorBlendAttachmentState> colorBlendAttachments, ArrayProxyRef<vk::DynamicState> dynamicStates)
{
    const auto viewportState = vk::PipelineViewportStateCreateInfo {
        {}, viewports.size(), viewports.data(), scissors.size(), scissors.data()
    };

    const auto colorBlendState = vk::PipelineColorBlendStateCreateInfo {
        {}, false, {}, colorBlendAttachments.size(), colorBlendAttachments.data(), {}
    };

    const auto dynamicState = vk::PipelineDynamicStateCreateInfo {
        {}, dynamicStates.size(), dynamicStates.data()
    };

    const auto result = ctx.device.createGraphicsPipeline (
        nullptr,
        vk::GraphicsPipelineCreateInfo {
            {}, uint(stages.size()), stages.data(), &vertexInputState, &inputAssemblyState, nullptr,
            &viewportState, &rasterizationState, &multisampleState, &depthStencilState, &colorBlendState,
            &dynamicState, layout, renderPass, subpass
        }
    );

    if (result.result != vk::Result::eSuccess)
        vk::throwResultException(result.result, "sq::vk_create_graphics_pipeline");

    return result.value;
}

//============================================================================//

vk::DescriptorSet sq::vk_allocate_descriptor_set(const VulkanContext& ctx, vk::DescriptorSetLayout layout)
{
    const auto info = vk::DescriptorSetAllocateInfo { ctx.descriptorPool, layout };

    vk::DescriptorSet descriptorSet;

    const auto result = ctx.device.allocateDescriptorSets(&info, &descriptorSet);
    if (result != vk::Result::eSuccess)
        vk::throwResultException(result, "sq::vk_allocate_descriptor_set");

    return descriptorSet;
}

Swapper<vk::DescriptorSet> sq::vk_allocate_descriptor_set_swapper(const VulkanContext& ctx, vk::DescriptorSetLayout layout)
{
    const auto layouts = std::array { layout, layout };
    const auto info = vk::DescriptorSetAllocateInfo { ctx.descriptorPool, layouts };

    Swapper<vk::DescriptorSet> swapper;

    const auto result = ctx.device.allocateDescriptorSets(&info, reinterpret_cast<vk::DescriptorSet*>(&swapper));
    if (result != vk::Result::eSuccess)
        vk::throwResultException(result, "sq::vk_allocate_descriptor_set_swapper");

    return swapper;
}
