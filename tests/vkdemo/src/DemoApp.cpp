#include "DemoApp.hpp"

#include "Resources.hpp"
#include "UniformBlocks.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/vk/Helpers.hpp>

#include <sqee/misc/Json.hpp>
#include <sqee/vk/Pipeline.hpp>

using namespace demo;

//============================================================================//

void DemoApp::initialise(std::vector<String> /*args*/)
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

    create_render_targets();
    create_pipelines();

    initialise_models();
}

//============================================================================//

DemoApp::~DemoApp()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.waitIdle();

    ctx.device.destroy(mCameraDescriptorSetLayout);
    ctx.device.destroy(mLightDescriptorSetLayout);
    ctx.device.destroy(mModelDescriptorSetLayout);
    ctx.device.destroy(mCompositeDescriptorSetLayout);

    ctx.device.destroy(mModelPipelineLayout);
    ctx.device.destroy(mCompositePipelineLayout);

    ctx.device.free(ctx.descriptorPool, {mCameraDescriptorSet.front, mCameraDescriptorSet.back});
    ctx.device.free(ctx.descriptorPool, {mLightDescriptorSet.front, mLightDescriptorSet.back});
    ctx.device.free(ctx.descriptorPool, mCompositeDescriptorSet);

    destroy_render_targets();
    destroy_pipelines();
}

//============================================================================//

void DemoApp::update(double elapsed)
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

void DemoApp::swap_resources()
{
    mCameraUbo.swap();
    mCameraDescriptorSet.swap();

    for (auto& model : mStaticModels)
    {
        model.ubo.swap();
        model.descriptorSet.swap();
    }
}

//============================================================================//

void DemoApp::handle_event(sq::Event event)
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

void DemoApp::refresh_graphics_config()
{
    mWindow->destroy_swapchain_and_friends();
    mWindow->create_swapchain_and_friends();

    destroy_render_targets();
    create_render_targets();

    destroy_pipelines();
    create_pipelines();

    mResourceCaches.pipelines.reload_resources();
    mResourceCaches.materials.reload_resources();
}

//============================================================================//

void DemoApp::initialise_layouts()
{
    const auto& ctx = sq::VulkanContext::get();

    // model pipeline
    {
        mCameraDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, nullptr
                }
            }
        );
        mLightDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
                }
            }
        );
        mModelDescriptorSetLayout = sq::vk_create_descriptor_set_layout (
            ctx, {}, {
                vk::DescriptorSetLayoutBinding {
                    0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr
                }
            }
        );

        mModelPipelineLayout = sq::vk_create_pipeline_layout (
            ctx, {}, { mCameraDescriptorSetLayout, mLightDescriptorSetLayout }, {}
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

        mCompositeDescriptorSet = sq::vk_allocate_descriptor_set(ctx, mCompositeDescriptorSetLayout);
    }
}

//============================================================================//

void DemoApp::initialise_camera()
{
    const auto& ctx = sq::VulkanContext::get();

    mCameraUbo.initialise(sizeof(CameraBlock), vk::BufferUsageFlagBits::eUniformBuffer);
    mCameraDescriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mCameraDescriptorSetLayout);

    sq::vk_update_descriptor_set_swapper (
        ctx, mCameraDescriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
        mCameraUbo.get_descriptor_info_front(), mCameraUbo.get_descriptor_info_back()
    );

    mLightUbo.initialise(sizeof(LightBlock), vk::BufferUsageFlagBits::eUniformBuffer);
    mLightDescriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mLightDescriptorSetLayout);

    sq::vk_update_descriptor_set_swapper (
        ctx, mLightDescriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
        mLightUbo.get_descriptor_info_front(), mLightUbo.get_descriptor_info_back()
    );
}

//============================================================================//

void DemoApp::initialise_models()
{
    const auto& ctx = sq::VulkanContext::get();

    // todo: more than one model
    {
        auto& model = mStaticModels.emplace_back();

        model.mesh = mResourceCaches.meshes.acquire("meshes/Dice");
        model.material = mResourceCaches.materials.acquire(sq::parse_json_from_file("assets/materials/Dice.json"));

        model.ubo.initialise(sizeof(StaticBlock), vk::BufferUsageFlagBits::eUniformBuffer);
        model.descriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mModelDescriptorSetLayout);

        sq::vk_update_descriptor_set_swapper (
            ctx, model.descriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
            model.ubo.get_descriptor_info_front(),
            model.ubo.get_descriptor_info_back()
        );
    }
}

//============================================================================//

void DemoApp::create_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    // create images and samplers
    {
        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
        {
            std::tie(mMsColourImage, mMsColourImageMem, mMsColourImageView) = sq::vk_create_image_2D (
                ctx, vk::Format::eB8G8R8A8Srgb, mWindow->get_size(), mMultisampleMode,
                false, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                false, {}, vk::ImageAspectFlagBits::eColor
            );
        }

        std::tie(mResolveColourImage, mResolveColourImageMem, mResolveColourImageView) = sq::vk_create_image_2D (
            ctx, vk::Format::eB8G8R8A8Srgb, mWindow->get_size(), vk::SampleCountFlagBits::e1,
            false, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            false, {}, vk::ImageAspectFlagBits::eColor
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
                    {}, mMsRenderPass, attachments, mWindow->get_size().x, mWindow->get_size().y, 1u
                }
            );
        }
        else // no multisample
        {
            const auto attachments = std::array { mResolveColourImageView };

            mMsFramebuffer = ctx.device.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mMsRenderPass, attachments, mWindow->get_size().x, mWindow->get_size().y, 1u
                }
            );
        }
    }

    mResourceCaches.passConfigMap = {
        { "Opaque", { mMsRenderPass, mMultisampleMode, mWindow->get_size() } }
    };
}

//============================================================================//

void DemoApp::destroy_render_targets()
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

void DemoApp::create_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    // composite pipeline and descriptor set
    {
        const auto shaderModules = sq::ShaderModules (
            ctx, "shaders/FullScreen.vert.spv", {}, "shaders/Composite.frag.spv"
        );

        mCompositePipeline = sq::vk_create_graphics_pipeline (
            ctx, mCompositePipelineLayout, mWindow->get_render_pass(), 0u, shaderModules.stages, {},
            vk::PipelineInputAssemblyStateCreateInfo {
                {}, vk::PrimitiveTopology::eTriangleList, false
            },
            vk::PipelineRasterizationStateCreateInfo {
                {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
                vk::FrontFace::eCounterClockwise, false, 0.f, false, 0.f, 1.f
            },
            vk::PipelineMultisampleStateCreateInfo {
                {}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false
            },
            vk::PipelineDepthStencilStateCreateInfo {
                {}, false, false, {}, false, false, {}, {}, 0.f, 0.f
            },
            vk::Viewport { 0.f, 0.f, float(mWindow->get_size().x), float(mWindow->get_size().y) },
            vk::Rect2D { {0, 0}, {mWindow->get_size().x, mWindow->get_size().y} },
            vk::PipelineColorBlendAttachmentState { false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111) },
            nullptr
        );

        sq::vk_update_descriptor_set (
            ctx, mCompositeDescriptorSet, 0u, 0u, vk::DescriptorType::eCombinedImageSampler,
            vk::DescriptorImageInfo {
                mResolveColourSampler, mResolveColourImageView, vk::ImageLayout::eShaderReadOnlyOptimal
            }
        );
    }
}

//============================================================================//

void DemoApp::destroy_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.destroy(mCompositePipeline);
}

//============================================================================//

void DemoApp::update_window_title(double elapsed)
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

void DemoApp::update_uniform_buffer(double elapsed)
{
    static double totalTime = 0.0;
    totalTime += elapsed * 0.2;

    static Mat4F modelMat;

    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Down))
        modelMat = Mat4F(maths::rotation(Vec3F(1.f, 0.f, 0.f), +float(elapsed) * 0.5f)) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Up))
        modelMat = Mat4F(maths::rotation(Vec3F(1.f, 0.f, 0.f), -float(elapsed) * 0.5f)) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Left))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, 1.f, 0.f), -float(elapsed) * 0.5f)) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Right))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, 1.f, 0.f), +float(elapsed) * 0.5f)) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::PageDown))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, 0.f, 1.f), -float(elapsed) * 0.5f)) * modelMat;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::PageUp))
        modelMat = Mat4F(maths::rotation(Vec3F(0.f, 0.f, 1.f), +float(elapsed) * 0.5f)) * modelMat;

    auto& camera = *reinterpret_cast<CameraBlock*>(mCameraUbo.map());
    {
        camera.viewMat = maths::look_at_LH(Vec3F(0.f, 0.f, -2.f), Vec3F(0.f, 0.f, 0.f), Vec3F(0.f, 1.f, 0.f));

        const float aspect = float(mWindow->get_size().x) / float(mWindow->get_size().y);
        camera.projMat = maths::perspective_LH(maths::radians(0.125f), aspect, 0.1f, 10.f);

        camera.invViewMat = maths::inverse(camera.viewMat);
        camera.invProjMat = maths::inverse(camera.projMat);
    }

    auto& light = *reinterpret_cast<LightBlock*>(mLightUbo.map());
    {
        light.ambiColour = { 0.3f, 0.3f, 0.3f };
        light.skyColour = { 0.7f, 0.7f, 0.7f };
        light.skyDirection = maths::normalize(Vec3F(0.f, 0.f, 1.f));
        light.skyMatrix = Mat4F();
    }

    for (auto& model : mStaticModels)
    {
        auto& block = *reinterpret_cast<StaticBlock*>(model.ubo.map());
        //block.matrix = camera.projMat * camera.viewMat * modelMat;
        block.matrix = modelMat;
        block.normMat = Mat34F(maths::normal_matrix(camera.viewMat * modelMat));
    }
}

//============================================================================//

void DemoApp::populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf)
{
    cmdbuf.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

    // render models to multisample fb
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mMsRenderPass, mMsFramebuffer, vk::Rect2D({0, 0}, {mWindow->get_size().x, mWindow->get_size().y})
            }, vk::SubpassContents::eInline
        );

        cmdbuf.clearAttachments (
            vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0u, vk::ClearValue(std::array{0.025f, 0.025f, 0.f, 1.f})),
            vk::ClearRect(vk::Rect2D({0, 0}, {mWindow->get_size().x, mWindow->get_size().y}), 0u, 1u)
        );

        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 0u, mCameraDescriptorSet.front, {});
        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 1u, mLightDescriptorSet.front, {});

        for (const auto& model : mStaticModels)
        {
            model.material->bind(cmdbuf);
            model.material->bind_final_descriptor_set(cmdbuf, model.descriptorSet.front);
            model.mesh->bind_buffers(cmdbuf);
            model.mesh->draw_complete(cmdbuf);
        }

        cmdbuf.endRenderPass();
    }

    // render resolve image and gui to window
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mWindow->get_render_pass(), framebuf, vk::Rect2D({0, 0}, {mWindow->get_size().x, mWindow->get_size().y})
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
