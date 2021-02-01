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

    mWindow->set_vsync_enabled(true);
    mWindow->create_swapchain_and_friends();

    mInputDevices = std::make_unique<sq::VulkInputDevices>(*mWindow);

    mGuiSystem = std::make_unique<sq::VulkGuiSystem>(*mWindow, *mInputDevices);
    mGuiSystem->set_style_colours_supertux();
    mGuiSystem->set_style_widgets_supertux();

    initialise_layouts();
    initialise_camera();
    initialise_models();

    create_render_targets();
    create_pipelines();
}

//============================================================================//

VulkTestApp::~VulkTestApp()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.waitIdle();

    ctx.device.destroy(mCameraDescriptorSetLayout);
    ctx.device.destroy(mModelDescriptorSetLayout);
    ctx.device.destroy(mCompositeDescriptorSetLayout);

    ctx.device.destroy(mModelPipelineLayout);
    ctx.device.destroy(mCompositePipelineLayout);

    destroy_render_targets();
    destroy_pipelines();
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

    auto [cmdbuf, framebuf] = mWindow->begin_frame();
    if (cmdbuf)
    {
        update_window_title(elapsed);
        update_uniform_buffer(elapsed);

        populate_command_buffer(cmdbuf, framebuf);

        mWindow->submit_present_swap();
        swap_resources();
    }
}

//============================================================================//

void VulkTestApp::swap_resources()
{
    mCameraUbo.swap();
    mCameraDescriptorSet.swap();

    for (auto& model : mModels)
    {
        model.ubo.swap();
        model.descriptorSet.swap();
    }
}

//============================================================================//

void VulkTestApp::handle_event(sq::Event event)
{
    if (event.type == sq::Event::Type::Window_Close)
        mReturnCode = 0;

    if (event.type == sq::Event::Type::Window_Resize)
        refresh_graphics_config();

    if (event.type == sq::Event::Type::Keyboard_Press)
    {
        if (event.data.keyboard.key == sq::Keyboard_Key::V)
        {
            mWindow->set_vsync_enabled(!mWindow->get_vsync_enabled());
            refresh_graphics_config();
        }

        if (event.data.keyboard.key == sq::Keyboard_Key::A)
        {
            mMultisampleMode = vk::SampleCountFlagBits(int(mMultisampleMode) * 2);
            if (int(mMultisampleMode) == 16) mMultisampleMode = vk::SampleCountFlagBits(1);
            refresh_graphics_config();
        }
    }
}

//============================================================================//

void VulkTestApp::refresh_graphics_config()
{
    mWindow->destroy_swapchain_and_friends();
    mWindow->create_swapchain_and_friends();

    destroy_render_targets();
    create_render_targets();

    destroy_pipelines();
    create_pipelines();
}

//============================================================================//

void VulkTestApp::initialise_layouts()
{
    const auto& ctx = sq::VulkanContext::get();

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
}

//============================================================================//

void VulkTestApp::initialise_camera()
{
    const auto& ctx = sq::VulkanContext::get();

    mCameraUbo.initialise(sizeof(CameraBlock), vk::BufferUsageFlagBits::eUniformBuffer);
    mCameraDescriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mCameraDescriptorSetLayout);

    sq::vk_update_descriptor_set_swapper (
        ctx, mCameraDescriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
        mCameraUbo.get_descriptor_info_front(),
        mCameraUbo.get_descriptor_info_back()
    );
}

//============================================================================//

void VulkTestApp::initialise_models()
{
    const auto& ctx = sq::VulkanContext::get();

    // todo: more than one model
    {
        auto& model = mModels.emplace_back();

        model.mesh.load_from_file("assets/meshes/Dice");
        model.texture.load_from_file_2D("assets/textures/Dice_diff");
        model.ubo.initialise(sizeof(ModelBlock), vk::BufferUsageFlagBits::eUniformBuffer);
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
}

//============================================================================//

void VulkTestApp::create_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    // create images and image views
    {
        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
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
        }

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
        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
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

            const auto dependencies = vk::SubpassDependency {
                0u, VK_SUBPASS_EXTERNAL,
                vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
                vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead,
                vk::DependencyFlagBits::eByRegion
            };

            mMsRenderPass = ctx.device.createRenderPass (
                vk::RenderPassCreateInfo { {}, attachments, subpasses, dependencies }
            );
        }
        else // no multisample
        {
            const auto attachments = std::array {
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

            const auto subpasses = vk::SubpassDescription {
                {}, vk::PipelineBindPoint::eGraphics, nullptr, colorAttachments, nullptr, nullptr, {}
            };

            const auto dependencies = vk::SubpassDependency {
                0u, VK_SUBPASS_EXTERNAL,
                vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
                vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead,
                vk::DependencyFlagBits::eByRegion
            };

            mMsRenderPass = ctx.device.createRenderPass (
                vk::RenderPassCreateInfo { {}, attachments, subpasses, dependencies }
            );
        }
    }

    // create ms framebuffer
    {
        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
        {
            const auto attachments = std::array { mMsColourImageView, mResolveColourImageView };

            mMsFramebuffer = ctx.device.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mMsRenderPass, attachments, ctx.window.size.x, ctx.window.size.y, 1u
                }
            );
        }
        else // no multisample
        {
            const auto attachments = std::array { mResolveColourImageView };

            mMsFramebuffer = ctx.device.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mMsRenderPass, attachments, ctx.window.size.x, ctx.window.size.y, 1u
                }
            );
        }
    }
}

//============================================================================//

void VulkTestApp::destroy_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    if (mMsColourImageMem) ctx.device.destroy(mMsColourImageView);
    if (mMsColourImageMem) ctx.device.destroy(mMsColourImage);
    if (mMsColourImageMem) mMsColourImageMem.free();

    ctx.device.destroy(mResolveColourSampler);
    ctx.device.destroy(mResolveColourImageView);
    ctx.device.destroy(mResolveColourImage);
    mResolveColourImageMem.free();

    ctx.device.destroy(mMsFramebuffer);
    ctx.device.destroy(mMsRenderPass);
}

//============================================================================//

void VulkTestApp::create_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    // model pipeline
    {
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
            vk::Viewport { 0.f, 0.f, float(ctx.window.size.x), float(ctx.window.size.y) },
            vk::Rect2D { {0, 0}, {ctx.window.size.x, ctx.window.size.y} },
            vk::PipelineColorBlendAttachmentState { false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111) },
            nullptr
        );
    }

    // composite pipeline
    {
        const auto shaderModules = sq::ShaderModules (
            ctx, "shaders/screen.vert.spv", {}, "shaders/screen.frag.spv"
        );

        mCompositePipeline = sq::vk_create_graphics_pipeline (
            ctx, mCompositePipelineLayout, mWindow->get_render_pass(), 0u, shaderModules.stages, {},
            vk::PipelineInputAssemblyStateCreateInfo {
                {}, vk::PrimitiveTopology::eTriangleList, false
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
            vk::Viewport { 0.f, 0.f, float(ctx.window.size.x), float(ctx.window.size.y) },
            vk::Rect2D { {0, 0}, {ctx.window.size.x, ctx.window.size.y} },
            vk::PipelineColorBlendAttachmentState { false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111) },
            nullptr
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

void VulkTestApp::destroy_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.destroy(mModelPipeline);
    ctx.device.destroy(mCompositePipeline);
}

//============================================================================//

void VulkTestApp::update_window_title(double elapsed)
{
    mTimeAccum += elapsed;
    mFramesAccum += 1u;

    if (mTimeAccum >= 0.5)
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

void VulkTestApp::populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf)
{
    const auto& ctx = sq::VulkanContext::get();

    cmdbuf.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

    // render models to multisample fb
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mMsRenderPass, mMsFramebuffer, vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y})
            }, vk::SubpassContents::eInline
        );

        cmdbuf.clearAttachments (
            vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0u, vk::ClearValue(std::array{0.05f, 0.05f, 0.f, 1.f})),
            vk::ClearRect(vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y}), 0u, 1u)
        );

        cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mModelPipeline);
        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 0u, mCameraDescriptorSet.front, {});

        for (const auto& model : mModels)
        {
            cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 1u, model.descriptorSet.front, {});
            model.mesh.bind_buffers(cmdbuf);
            model.mesh.draw_complete(cmdbuf);
        }

        cmdbuf.endRenderPass();
    }

    // render resolve image and gui to window
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mWindow->get_render_pass(), framebuf, vk::Rect2D({0, 0}, {ctx.window.size.x, ctx.window.size.y})
            }, vk::SubpassContents::eInline
        );

        cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mCompositePipeline);
        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mCompositePipelineLayout, 0u, mCompositeDescriptorSet, {});

        cmdbuf.draw(3u, 1u, 0u, 0u);

        mGuiSystem->render_gui(cmdbuf);

        cmdbuf.endRenderPass();
    }

    cmdbuf.end();
}
