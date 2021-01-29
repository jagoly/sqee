#include "VulkTestApp.hpp"

#include "UniformBlocks.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/vk/Helpers.hpp>

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

void VulkTestApp::initialise(std::vector<String> /*args*/)
{
    mWindow = std::make_unique<sq::VulkWindow> (
        "Hello Vulkan", Vec2U(800u, 600u), "sqee-vkdemo", Vec3U(0u, 0u, 1u)
    );
    mWindow->create_swapchain_and_friends(true);
    mWindow->set_vsync_enabled(true);

    mInputDevices = std::make_unique<sq::VulkInputDevices>(*mWindow);

    mGuiSystem = std::make_unique<sq::VulkGuiSystem>(*mWindow, *mInputDevices);
    mGuiSystem->set_style_colours_supertux();
    mGuiSystem->set_style_widgets_supertux();

    create_layouts();
    create_render_targets();
    create_objects();
    create_descriptor_sets();

    create_pipelines();
}

//============================================================================//

VulkTestApp::~VulkTestApp()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.waitIdle();

    ctx.device.destroy(mModelDescriptorSetLayout);

    ctx.device.destroy(mModelPipeline);
    ctx.device.destroy(mModelPipelineLayout);

    destroy_render_targets();
}

//============================================================================//

void VulkTestApp::update(double elapsed)
{
    if (mWindow->has_focus())
    {
        for (const auto& event : mWindow->fetch_events())
            if (!mGuiSystem->handle_event(event))
                handle_event(event);

        mGuiSystem->finish_handle_events(true);
    }
    else mGuiSystem->finish_handle_events(false);

    mGuiSystem->show_imgui_demo();
    mGuiSystem->finish_scene_update(elapsed);

    auto [cmdbuf, imageIndex] = mWindow->begin_new_frame();
    if (cmdbuf)
    {
        update_window_title(elapsed);
        update_uniform_buffer(elapsed);

        populate_command_buffer(cmdbuf, imageIndex);

        mWindow->submit_and_present();

        mCameraUbo.swap();
        mCameraDescriptorSet.swap();
        for (auto& model : mModels) model.ubo.swap();
    }
}

//============================================================================//

void VulkTestApp::handle_event(sq::Event event)
{
    if (event.type == sq::Event::Type::Window_Close)
        mReturnCode = 0;

    if (event.type == sq::Event::Type::Window_Resize)
        handle_window_resize();

    if (event.type == sq::Event::Type::Keyboard_Press)
    {
        if (event.data.keyboard.key == sq::Keyboard_Key::V)
        {
            //constexpr const auto STRINGS = std::array { "OFF", "ON" };
            const bool newValue = !mWindow->get_vsync_enabled();
            //mDebugOverlay->notify(sq::build_string("vsync set to ", STRINGS[newValue]));
            mWindow->set_vsync_enabled(newValue);
        }
    }
}

//============================================================================//

void VulkTestApp::handle_window_resize()
{
    mWindow->destroy_swapchain_and_friends();
    mWindow->create_swapchain_and_friends(true);

    destroy_render_targets();
    create_render_targets();

    destroy_pipelines();
    create_pipelines();
}

//============================================================================//

void VulkTestApp::create_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    if (mMultisampleMode == vk::SampleCountFlagBits::e1)
        return; // EARLY RETURN

    // create images and image views
    {
        std::tie(mMsColourImage, mMsColourImageMem) = sq::vk_create_image_2D (
            ctx, vk::Format::eB8G8R8A8Srgb, ctx.window.size, mMultisampleMode, false,
            vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, false
        );

        mMsColourImageView = ctx.device.createImageView (
            vk::ImageViewCreateInfo {
                {}, mMsColourImage, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, {},
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
            }
        );

        std::tie(mResolveColourImage, mResolveColourImageMem) = sq::vk_create_image_2D (
            ctx, vk::Format::eB8G8R8A8Srgb, ctx.window.size, vk::SampleCountFlagBits::e1, false,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, false
        );

        mResolveColourImageView = ctx.device.createImageView (
            vk::ImageViewCreateInfo {
                {}, mResolveColourImage, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, {},
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
            }
        );

        mResolveColourSampler = ctx.device.createSampler (
            vk::SamplerCreateInfo {
                {}, vk::Filter::eNearest, vk::Filter::eNearest, vk::SamplerMipmapMode::eNearest,
                vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
                0.f, false, 0.f, false, {}, 0.f, 0.f, {}, false
            }
        );
    }

    // create ms render pass
    {
        const auto attachments = std::array {
            vk::AttachmentDescription {
                {}, vk::Format::eB8G8R8A8Srgb, mMultisampleMode,
                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal
            },
            vk::AttachmentDescription {
                {}, vk::Format::eB8G8R8A8Srgb, vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal
            }
        };

        const auto colorAttachments = std::array {
            vk::AttachmentReference { 0u, vk::ImageLayout::eColorAttachmentOptimal }
        };

        const auto resolveAttachments = std::array {
            vk::AttachmentReference { 1u, vk::ImageLayout::eColorAttachmentOptimal }
        };

        const auto subpasses = vk::SubpassDescription {
            {}, vk::PipelineBindPoint::eGraphics, nullptr, colorAttachments, resolveAttachments, nullptr, {}
        };

        const auto dependencies = std::array {
            vk::SubpassDependency {
                VK_SUBPASS_EXTERNAL, 0u,
                vk::PipelineStageFlagBits::eBottomOfPipe,
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::AccessFlagBits::eMemoryRead,
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
                vk::DependencyFlagBits::eByRegion
            },
            vk::SubpassDependency {
                0u, VK_SUBPASS_EXTERNAL,
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eBottomOfPipe,
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
                vk::AccessFlagBits::eMemoryRead,
                vk::DependencyFlagBits::eByRegion
            }
        };

        mMsRenderPass = ctx.device.createRenderPass (
            vk::RenderPassCreateInfo {
                {}, attachments, subpasses, dependencies
            }
        );
    }

    // create ms framebuffer
    {
        const auto attachments = std::array { mMsColourImageView, mResolveColourImageView };

        mMsFramebuffer = ctx.device.createFramebuffer (
            vk::FramebufferCreateInfo {
                {}, mMsRenderPass, attachments, ctx.window.size.x, ctx.window.size.y, 1u
            }
        );
    }
}

//============================================================================//

void VulkTestApp::destroy_render_targets()
{
    // todo
}

//============================================================================//

void VulkTestApp::create_objects()
{
    mCameraUbo.initialise(sizeof(CameraBlock), vk::BufferUsageFlagBits::eUniformBuffer);

    auto& model = mModels.emplace_back();

    model.mesh.load_from_file("assets/meshes/Dice");
    model.texture.load_from_file_2D("assets/textures/Dice_diff");
    model.ubo.initialise(sizeof(ModelBlock), vk::BufferUsageFlagBits::eUniformBuffer);
}

//============================================================================//

void VulkTestApp::create_layouts()
{
    const auto& ctx = sq::VulkanContext::get();
  #if 0
    // camera descriptor set layout
    {
        auto bindings = std::array {
            vk::DescriptorSetLayoutBinding {
                0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr
            }
        };

        mCameraDescriptorSetLayout = ctx.device.createDescriptorSetLayout (
            vk::DescriptorSetLayoutCreateInfo { {}, bindings }
        );
    }

    // model descriptor set layout
    {
        auto bindings = std::array {
            vk::DescriptorSetLayoutBinding {
                0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr
            },
            vk::DescriptorSetLayoutBinding {
                1u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
            }
        };

        mModelDescriptorSetLayout = ctx.device.createDescriptorSetLayout (
            vk::DescriptorSetLayoutCreateInfo { {}, bindings }
        );
    }

    // model pipeline layout
    {
        auto setLayouts = std::array { mCameraDescriptorSetLayout, mModelDescriptorSetLayout };

        mModelPipelineLayout = ctx.device.createPipelineLayout (
            vk::PipelineLayoutCreateInfo { {}, setLayouts, {} }
        );
    }

    // composite descriptor set layout
    {
        auto bindings = std::array {
            vk::DescriptorSetLayoutBinding {
                0u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
            }
        };

        mCompositeDescriptorSetLayout = ctx.device.createDescriptorSetLayout (
            vk::DescriptorSetLayoutCreateInfo { {}, bindings }
        );
    }

    // composite pipeline layout
    {
        mCompositePipelineLayout = ctx.device.createPipelineLayout (
            vk::PipelineLayoutCreateInfo { {}, mCompositeDescriptorSetLayout, {} }
        );
    }
  #else
    // model pipeline
    {
        mCameraDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr
                }
            }
        );

        mModelDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex
                },
                vk::DescriptorSetLayoutBinding {
                    1u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment
                }
            }
        );

        mModelPipelineLayout = sq::vk_create_pipeline_layout (
            ctx, {}, { mCameraDescriptorSetLayout, mModelDescriptorSetLayout }, {}
        );
    }

    // composite pipeline
    {
        mCompositeDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
                }
            }
        );

        mCompositePipelineLayout = sq::vk_create_pipeline_layout (
            ctx, {}, { mCompositeDescriptorSetLayout }, {}
        );
    }
  #endif
}

//============================================================================//

void VulkTestApp::create_descriptor_sets()
{
    const auto& ctx = sq::VulkanContext::get();

    // camera descriptor sets
    {
        mCameraDescriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mCameraDescriptorSetLayout);

        sq::vk_update_descriptor_set_swapper (
            ctx, mCameraDescriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
            mCameraUbo.get_descriptor_info_front(),
            mCameraUbo.get_descriptor_info_back()
        );
    }

    for (auto& model : mModels) // model descriptor sets
    {
        model.descriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mModelDescriptorSetLayout);

        sq::vk_update_descriptor_set_swapper (
            ctx, model.descriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
            model.ubo.get_descriptor_info_front(),
            model.ubo.get_descriptor_info_back()
        );

        sq::vk_update_descriptor_set_swapper (
            ctx, model.descriptorSet, 1u, 0u, vk::DescriptorType::eCombinedImageSampler,
            model.texture.get_descriptor_info(),
            model.texture.get_descriptor_info()
        );
    }

    // composite descriptor set
    {
        mCompositeDescriptorSet = sq::vk_allocate_descriptor_set(ctx, mCompositeDescriptorSetLayout);

        sq::vk_update_descriptor_set (
            ctx, mCompositeDescriptorSet, 0u, 0u, vk::DescriptorType::eCombinedImageSampler,
            vk::DescriptorImageInfo {
                mResolveColourSampler, mResolveColourImageView, vk::ImageLayout::eShaderReadOnlyOptimal
            }
        );
    }
}

//============================================================================//

void VulkTestApp::create_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    // model pipeline
    {
      #if 0

        const auto shaderModules = sq::ShaderModules (
            ctx, "shaders/object.vert.spv", {}, "shaders/object.frag.spv"
        );

        const auto vertexConfig = sq::VulkMesh::VertexConfig (
            sq::VulkMesh::Attribute::TexCoords | sq::VulkMesh::Attribute::Normals | sq::VulkMesh::Attribute::Tangents
        );

        const auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

        const auto viewportState = vk::PipelineViewportStateCreateInfo {
            {}, 1u, nullptr, 1u, nullptr
        };

        const auto rasterizationState = vk::PipelineRasterizationStateCreateInfo {
            {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
            vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
        };

        const auto multisampleState = vk::PipelineMultisampleStateCreateInfo {
            {}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false
        };

        const auto attachments = std::array {
            vk::PipelineColorBlendAttachmentState { false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111) }
        };
        const auto colorBlendState = vk::PipelineColorBlendStateCreateInfo {
            {}, false, {}, attachments, {}
        };

        auto states = std::array { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        auto dynamicState = vk::PipelineDynamicStateCreateInfo { {}, states };

        auto result = ctx.device.createGraphicsPipeline (
            nullptr,
            vk::GraphicsPipelineCreateInfo {
                {},
                shaderModules.stages, &vertexConfig.state, &inputAssemblyState, nullptr,
                &viewportState, &rasterizationState, &multisampleState, nullptr, &colorBlendState,
                &dynamicState, mModelPipelineLayout, mMsRenderPass, 0u
            }
        );
        SQASSERT(result.result == vk::Result::eSuccess, "");

      #else

        const auto shaderModules = sq::ShaderModules (
            ctx, "shaders/object.vert.spv", {}, "shaders/object.frag.spv"
        );

        const auto vertexConfig = sq::VulkMesh::VertexConfig (
            sq::VulkMesh::Attribute::TexCoords | sq::VulkMesh::Attribute::Normals | sq::VulkMesh::Attribute::Tangents
        );

        mModelPipeline = sq::vk_create_graphics_pipeline (
            ctx, mModelPipelineLayout, mMsRenderPass, 0u, shaderModules.stages, vertexConfig.state,
            vk::PipelineInputAssemblyStateCreateInfo {
                {}, vk::PrimitiveTopology::eTriangleList, false
            },
            vk::PipelineViewportStateCreateInfo {
                {}, 1u, nullptr, 1u, nullptr
            },
            vk::PipelineRasterizationStateCreateInfo {
                {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
                vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
            },
            vk::PipelineMultisampleStateCreateInfo {
                {}, mMultisampleMode, false, 0.f, nullptr, false, false
            },
            vk::PipelineDepthStencilStateCreateInfo {
                {}, false, false, {}, false, false, {}, {}, 0.f, 0.f
            },
            vk::PipelineColorBlendAttachmentState {
                false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
            },
            { vk::DynamicState::eViewport, vk::DynamicState::eScissor }
        );

      #endif
    }

    // composite pipeline
    {
        const auto shaderModules = sq::ShaderModules (
            ctx, "shaders/screen.vert.spv", {}, "shaders/screen.frag.spv"
        );

        mCompositePipeline = sq::vk_create_graphics_pipeline (
            ctx, mCompositePipelineLayout, mWindow->get_render_pass(), 0u, shaderModules.stages,
            vk::PipelineVertexInputStateCreateInfo(),
            vk::PipelineInputAssemblyStateCreateInfo {
                {}, vk::PrimitiveTopology::eTriangleList, false
            },
            vk::PipelineViewportStateCreateInfo {
                {}, 1u, nullptr, 1u, nullptr
            },
            vk::PipelineRasterizationStateCreateInfo {
                {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
                vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
            },
            vk::PipelineMultisampleStateCreateInfo {
                {}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false
            },
            vk::PipelineDepthStencilStateCreateInfo {
                {}, false, false, {}, false, false, {}, {}, 0.f, 0.f
            },
            vk::PipelineColorBlendAttachmentState {
                false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
            },
            { vk::DynamicState::eViewport, vk::DynamicState::eScissor }
        );
    }
}

//============================================================================//

void VulkTestApp::destroy_pipelines()
{
    // todo
}

//============================================================================//

void VulkTestApp::update_window_title(double elapsed)
{
    mTimeAccum += elapsed;
    mFramesAccum += 1u;

    if (mTimeAccum >= 1.0)
    {
        const double avgTime = mTimeAccum * 1000.0 / double(mFramesAccum);
        const double avgFps = 1.0 / (mTimeAccum / double(mFramesAccum));
        mWindow->set_title(fmt::format("{:.4f} ms | {:.2f} fps", avgTime, avgFps));
        mTimeAccum = 0.0;
        mFramesAccum = 0u;
    }
}

//============================================================================//

void VulkTestApp::update_uniform_buffer(double elapsed)
{
    const auto& ctx = sq::VulkanContext::get();

    static double totalTime = 0.0;
    totalTime += elapsed * 0.2;

    static Mat4F modelMat = Mat4F();

    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Up))
        modelMat = Mat4F(maths::rotation(Vec3F(1.f, 0.f, 0.f), float(elapsed))) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Down))
        modelMat = Mat4F(maths::rotation(Vec3F(-1.f, 0.f, 0.f), float(elapsed))) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Left))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, 1.f, 0.f), float(elapsed))) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Right))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, -1.f, 0.f), float(elapsed))) * modelMat;

    for (auto& model : mModels)
    {
        auto& block = *static_cast<ModelBlock*>(model.ubo.map());
        block.model = modelMat;
    }

    // update camera block
    {
        auto& block = *static_cast<CameraBlock*>(mCameraUbo.map());

        block.view = maths::look_at_RH(Vec3F(0.f, 0.1f, -2.f), Vec3F(0.f, 0.f, 0.f), Vec3F(0.f, 1.f, 0.f));

        const float aspect = float(ctx.window.size.x) / float(ctx.window.size.y);
        block.proj = maths::perspective_RH(maths::radians(0.125f), aspect, 0.1f, 10.f);
    }
}

//============================================================================//

void VulkTestApp::populate_command_buffer(vk::CommandBuffer cmdbuf, uint32_t /*imageIndex*/)
{
    const auto& ctx = sq::VulkanContext::get();

    cmdbuf.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

    // render models to multisample fb
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mMsRenderPass, mMsFramebuffer,
                vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y}),
                nullptr
            },
            vk::SubpassContents::eInline
        );

        cmdbuf.clearAttachments (
            vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0u, vk::ClearValue(std::array{0.05f, 0.05f, 0.f, 1.f})),
            vk::ClearRect(vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y}), 0u, 1u)
        );

        cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mModelPipeline);

        cmdbuf.setViewport(0u, vk::Viewport(0.f, 0.f, float(ctx.window.size.x), float(ctx.window.size.y), 0.f, 1.f));
        cmdbuf.setScissor(0u, vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y}));

        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 0u, mCameraDescriptorSet.front, {});

        for (const auto& model : mModels)
        {
            model.mesh.bind_buffers(cmdbuf);
            cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 1u, model.descriptorSet.front, {});

            model.mesh.draw_complete(cmdbuf);
        }

        cmdbuf.endRenderPass();
    }

    // render resolve image and gui to window
    {
        mWindow->begin_render_pass(cmdbuf);

        cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mCompositePipeline);

        cmdbuf.setViewport(0u, vk::Viewport(0.f, 0.f, float(ctx.window.size.x), float(ctx.window.size.y), 0.f, 1.f));
        cmdbuf.setScissor(0u, vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y}));
    
        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mCompositePipelineLayout, 0u, mCompositeDescriptorSet, {});

        cmdbuf.draw(3u, 1u, 0u, 0u);
    
        mGuiSystem->render_gui(cmdbuf);
    
        cmdbuf.endRenderPass();
    }

    cmdbuf.end();
}
