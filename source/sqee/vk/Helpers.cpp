#include <sqee/vk/Helpers.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

//============================================================================//

StagingBuffer::StagingBuffer(const VulkanContext& _ctx, size_t size)
    : ctx(_ctx)
{
    buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, {}
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

ShaderModules::ShaderModules(const VulkanContext& _ctx, std::optional<String> vertex, std::optional<String> geometry, std::optional<String> fragment)
    : ctx(_ctx)
{
    modules.reserve(size_t(vertex.has_value() + geometry.has_value() + fragment.has_value()));
    stages.reserve(size_t(vertex.has_value() + geometry.has_value() + fragment.has_value()));

    const auto load_shader = [this](const String& source, vk::ShaderStageFlagBits stage)
    {
        auto shaderCode = sq::get_bytes_from_file(source);

        auto shaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo { {}, shaderCode.size(), reinterpret_cast<uint32_t*>(shaderCode.data()) }
        );

        auto shaderStage = vk::PipelineShaderStageCreateInfo { {}, stage, shaderModule, "main", nullptr };

        modules.push_back(shaderModule);
        stages.push_back(shaderStage);
    };

    if (vertex.has_value()) load_shader(*vertex, vk::ShaderStageFlagBits::eVertex);
    if (geometry.has_value()) load_shader(*geometry, vk::ShaderStageFlagBits::eGeometry);
    if (fragment.has_value()) load_shader(*fragment, vk::ShaderStageFlagBits::eFragment);
}

ShaderModules::ShaderModules(const VulkanContext& _ctx, std::optional<Span> vertex, std::optional<Span> geometry, std::optional<Span> fragment)
    : ctx(_ctx)
{
    modules.reserve(size_t(vertex.has_value() + geometry.has_value() + fragment.has_value()));
    stages.reserve(size_t(vertex.has_value() + geometry.has_value() + fragment.has_value()));

    const auto load_shader = [this](const Span& source, vk::ShaderStageFlagBits stage)
    {
        auto shaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo { {}, std::get<1>(source), std::get<0>(source) }
        );

        auto shaderStage = vk::PipelineShaderStageCreateInfo { {}, stage, shaderModule, "main", nullptr };

        modules.push_back(shaderModule);
        stages.push_back(shaderStage);
    };

    if (vertex.has_value()) load_shader(*vertex, vk::ShaderStageFlagBits::eVertex);
    if (geometry.has_value()) load_shader(*geometry, vk::ShaderStageFlagBits::eGeometry);
    if (fragment.has_value()) load_shader(*fragment, vk::ShaderStageFlagBits::eFragment);
}

ShaderModules::~ShaderModules()
{
    for (auto& module : modules)
        ctx.device.destroy(module);
}

//============================================================================//

std::tuple<vk::Buffer, VulkanMemory> sq::vk_create_buffer(const VulkanContext& ctx, size_t size, vk::BufferUsageFlags usage, bool host)
{
    auto buffer = ctx.device.createBuffer (
        vk::BufferCreateInfo {
            {}, size, usage, vk::SharingMode::eExclusive, {}
        }
    );

    auto memory = ctx.allocator.allocate(ctx.device.getBufferMemoryRequirements(buffer), host);
    ctx.device.bindBufferMemory(buffer, memory.get_memory(), memory.get_offset());

    return { buffer, memory };
}

//============================================================================//

std::tuple<vk::Image, VulkanMemory> sq::vk_create_image_2D(const VulkanContext& ctx, vk::Format format, Vec2U size, vk::SampleCountFlagBits samples, bool linear, vk::ImageUsageFlags usage, bool host)
{
    auto image = ctx.device.createImage (
        vk::ImageCreateInfo {
            {}, vk::ImageType::e2D, format, vk::Extent3D(size.x, size.y, 1u),
            1u, 1u, samples,
            linear ? vk::ImageTiling::eLinear : vk::ImageTiling::eOptimal, usage,
            vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined
        }
    );

    auto memory = ctx.allocator.allocate(ctx.device.getImageMemoryRequirements(image), host);
    ctx.device.bindImageMemory(image, memory.get_memory(), memory.get_offset());

    return { image, memory };
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
        vk::throwResultException(result.result, "sq::vk_create_gfx_pipeline");

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

    const auto result = ctx.device.allocateDescriptorSets(&info, &swapper.front);
    if (result != vk::Result::eSuccess)
        vk::throwResultException(result, "sq::vk_allocate_descriptor_set_swapper");

    return swapper;
}
