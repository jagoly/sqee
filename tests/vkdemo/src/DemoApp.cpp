#include "DemoApp.hpp"

#include "Resources.hpp"
#include "UniformBlocks.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/vk/Helpers.hpp>

using namespace demo;

//==============================================================================

void DemoApp::initialise(std::vector<String> /*args*/)
{
    mWindow = std::make_unique<sq::Window> (
        "Hello Vulkan", Vec2U(800u, 600u), "sqee-vkdemo", Vec3U(0u, 0u, 1u)
    );

    mWindow->set_size_limits(Vec2U(200u, 150u), std::nullopt);
    mWindow->set_vsync_enabled(true);
    mWindow->create_swapchain_and_friends();

    mInputDevices = std::make_unique<sq::InputDevices>(*mWindow);

    mGuiSystem = std::make_unique<sq::GuiSystem>(*mWindow, *mInputDevices);
    mGuiSystem->set_style_colours_supertux();
    mGuiSystem->set_style_widgets_supertux();

    mCaches = std::make_unique<ResourceCaches>();

    mPassConfigOpaque = &mCaches->passConfigMap["Opaque"];

    initialise_layouts();
    initialise_camera();

    create_render_targets();
    create_pipelines();

    initialise_models();
}

//==============================================================================

DemoApp::~DemoApp()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.waitIdle();

    ctx.device.destroy(mModelDescriptorSetLayout);
    ctx.device.destroy(mCompositeDescriptorSetLayout);

    ctx.device.destroy(mModelPipelineLayout);
    ctx.device.destroy(mCompositePipelineLayout);

    ctx.device.free(ctx.descriptorPool, {mModelDescriptorSet.front, mModelDescriptorSet.back});
    ctx.device.free(ctx.descriptorPool, mCompositeDescriptorSet);

    destroy_render_targets();
    destroy_pipelines();
}

//==============================================================================

void DemoApp::update(double elapsed)
{
    const bool hasFocus = mWindow->has_focus();

    for (const auto& event : mWindow->fetch_events())
        if (mGuiSystem->handle_event(event) == false && hasFocus == true)
            handle_event(event);

    mGuiSystem->finish_handle_events();

    mWindow->begin_frame();

    update_window_title(elapsed);
    update_uniform_buffer(elapsed);

    mGuiSystem->show_imgui_demo();
    mGuiSystem->finish_scene_update(elapsed);

    auto [cmdbuf, framebuf] = mWindow->acquire_image();
    if (cmdbuf)
    {
        populate_command_buffer(cmdbuf, framebuf);
        mWindow->submit_present_swap();
    }
}

//==============================================================================

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

//==============================================================================

void DemoApp::refresh_graphics_config()
{
    const auto& ctx = sq::VulkanContext::get();

    mWindow->destroy_swapchain_and_friends();
    destroy_render_targets();
    destroy_pipelines();

    ctx.allocator.free_empty_blocks();

    mWindow->create_swapchain_and_friends();
    create_render_targets();
    create_pipelines();

    mCaches->pipelines.reload_resources();

    sq::log_debug("vulkan memory usage | host = {:.1f}MiB | device = {:.1f}MiB",
                  float(ctx.allocator.get_memory_usage(true)) / 1048576.f,
                  float(ctx.allocator.get_memory_usage(false)) / 1048576.f);
}

//==============================================================================

void DemoApp::initialise_layouts()
{
    const auto& ctx = sq::VulkanContext::get();

    // model pipeline
    {
        mModelDescriptorSetLayout = ctx.create_descriptor_set_layout ({
            vk::DescriptorSetLayoutBinding {
                0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
            },
            vk::DescriptorSetLayoutBinding {
                1u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eFragment
            },
            vk::DescriptorSetLayoutBinding {
                2u, vk::DescriptorType::eStorageBuffer, 1u, vk::ShaderStageFlagBits::eVertex
            }
        });

        const auto pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0u, 128u);

        mModelPipelineLayout = ctx.create_pipeline_layout (
            {mModelDescriptorSetLayout, mCaches->bindlessTextureSetLayout}, pushConstantRange
        );
    }

    // composite pipeline
    {
        mCompositeDescriptorSetLayout = ctx.create_descriptor_set_layout ({
            vk::DescriptorSetLayoutBinding {
                0u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
            }
        });

        mCompositePipelineLayout = ctx.create_pipeline_layout(mCompositeDescriptorSetLayout, {});

        mCompositeDescriptorSet = sq::vk_allocate_descriptor_set(ctx, mCompositeDescriptorSetLayout);
    }
}

//==============================================================================

void DemoApp::initialise_camera()
{
    const auto& ctx = sq::VulkanContext::get();

    mCameraUbo.initialise(sizeof(CameraBlock), vk::BufferUsageFlagBits::eUniformBuffer);
    mLightUbo.initialise(sizeof(LightBlock), vk::BufferUsageFlagBits::eUniformBuffer);
    mMatrixUbo.initialise(65536u, vk::BufferUsageFlagBits::eStorageBuffer);

    mModelDescriptorSet = sq::vk_allocate_descriptor_set_swapper(ctx, mModelDescriptorSetLayout);

    sq::vk_update_descriptor_set_swapper (
        ctx, mModelDescriptorSet,
        sq::DescriptorUniformBuffer(0u, 0u, mCameraUbo.get_descriptor_info()),
        sq::DescriptorUniformBuffer(1u, 0u, mLightUbo.get_descriptor_info()),
        sq::DescriptorStorageBuffer(2u, 0u, mMatrixUbo.get_descriptor_info())
    );
}

//==============================================================================

void DemoApp::initialise_models()
{
    // todo: load a bunch of DrawItems from a Render.json file
    {
        auto& model = mStaticModels.emplace_back();

        model.mesh = mCaches->meshes.acquire("meshes/Dice");
        model.texDiffuse = mCaches->textures.acquire("textures/Dice_diff");
        model.texSpecular = mCaches->textures.acquire("textures/Gray60");
        model.texNormal = mCaches->textures.acquire("textures/Dice_norm");
        model.pipeline = mCaches->pipelines.acquire(sq::JsonDocument::parse_file("assets/pipelines/Dice.json").root().dump(false));
    }
}

//==============================================================================

void DemoApp::create_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    // create images and samplers
    {
        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
        {
            mMsColourImage.initialise_2D (
                ctx, vk::Format::eB8G8R8A8Srgb, mWindow->get_size(), 1u, mMultisampleMode,
                vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
                false, {}, vk::ImageAspectFlagBits::eColor
            );
        }

        mResolveColourImage.initialise_2D (
            ctx, vk::Format::eB8G8R8A8Srgb, mWindow->get_size(), 1u, vk::SampleCountFlagBits::e1,
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            false, {}, vk::ImageAspectFlagBits::eColor
        );

        mResolveColourSampler = ctx.create_sampler (
            vk::Filter::eNearest, vk::Filter::eNearest, {},
            vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, {},
            0.f, 0u, 0u, false, false, {}
        );
    }

    // create ms render pass
    {
        const auto dependencies = std::array {
            vk::SubpassDependency {
                VK_SUBPASS_EXTERNAL, 0u,
                vk::PipelineStageFlagBits::eFragmentShader, vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite,
                vk::DependencyFlagBits::eByRegion
            },
            vk::SubpassDependency {
                0u, VK_SUBPASS_EXTERNAL,
                vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
                vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead,
                vk::DependencyFlagBits::eByRegion
            }
        };

        if (mMultisampleMode > vk::SampleCountFlagBits::e1)
        {
            const auto attachments = std::array {
                vk::AttachmentDescription {
                    {}, vk::Format::eB8G8R8A8Srgb, mMultisampleMode,
                    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
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

            const auto colourReference  = vk::AttachmentReference { 0u, vk::ImageLayout::eColorAttachmentOptimal };
            const auto resolveReference = vk::AttachmentReference { 1u, vk::ImageLayout::eColorAttachmentOptimal };

            const auto subpass = vk::SubpassDescription {
                {}, vk::PipelineBindPoint::eGraphics, nullptr, colourReference, resolveReference, nullptr, nullptr
            };

            mMsRenderPass.initialise (
                ctx, attachments, subpass, dependencies, mWindow->get_size(), 1u, { mMsColourImage.view, mResolveColourImage.view }
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

            const auto colourReference = vk::AttachmentReference { 0u, vk::ImageLayout::eColorAttachmentOptimal };

            const auto subpass = vk::SubpassDescription {
                {}, vk::PipelineBindPoint::eGraphics, nullptr, colourReference, nullptr, nullptr, nullptr
            };

            mMsRenderPass.initialise (
                ctx, attachments, subpass, dependencies, mWindow->get_size(), 1u, mResolveColourImage.view
            );
        }
    }

    *mPassConfigOpaque = sq::PassConfig {
        mMsRenderPass.pass, 0u, mMultisampleMode, {}, mWindow->get_size(), mModelPipelineLayout, {}
    };
}

//==============================================================================

void DemoApp::destroy_render_targets()
{
    const auto& ctx = sq::VulkanContext::get();

    if (mMsColourImage.memory)
        mMsColourImage.destroy(ctx);

    mResolveColourImage.destroy(ctx);
    ctx.device.destroy(mResolveColourSampler);
    mMsRenderPass.destroy(ctx);
}

//==============================================================================

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
            ctx, mCompositeDescriptorSet,
            sq::DescriptorImageSampler(0u, 0u, mResolveColourSampler, mResolveColourImage.view, vk::ImageLayout::eShaderReadOnlyOptimal)
        );
    }
}

//==============================================================================

void DemoApp::destroy_pipelines()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.destroy(mCompositePipeline);
}

//==============================================================================

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

//==============================================================================

void DemoApp::update_uniform_buffer(double elapsed)
{
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

    if (mInputDevices->is_pressed(sq::Keyboard_Key::LeftBracket))
        modelMat = Mat4F(maths::scale(modelMat, Vec3F(1.f, 1.f, 1.f - float(elapsed))));
    if (mInputDevices->is_pressed(sq::Keyboard_Key::RightBracket))
        modelMat = Mat4F(maths::scale(modelMat, Vec3F(1.f, 1.f, 1.f / (1.f - float(elapsed)))));

    mModelDescriptorSet.swap();

    auto& camera = *reinterpret_cast<CameraBlock*>(mCameraUbo.swap_map());
    {
        camera.viewMat = maths::look_at_LH(Vec3F(0.f, 0.f, -2.f), Vec3F(0.f, 0.f, 0.f), Vec3F(0.f, 1.f, 0.f));

        const float aspect = float(mWindow->get_size().x) / float(mWindow->get_size().y);
        camera.projMat = maths::perspective_LH(maths::radians(0.125f), aspect, 0.1f, 10.f);

        camera.invViewMat = maths::inverse(camera.viewMat);
        camera.invProjMat = maths::inverse(camera.projMat);
    }

    auto& light = *reinterpret_cast<LightBlock*>(mLightUbo.swap_map());
    {
        light.ambiColour = { 0.3f, 0.3f, 0.3f };
        light.skyColour = { 0.7f, 0.7f, 0.7f };
        light.skyDirection = maths::normalize(Vec3F(0.f, 0.f, 1.f));
        light.skyMatrix = Mat4F();
    }

    Mat34F* mats = reinterpret_cast<Mat34F*>(mMatrixUbo.swap_map());
    int matIndex = -1;

    for (auto& model : mStaticModels)
    {
        model.modelMatIndex = ++matIndex;
        mats[matIndex] = Mat34F(maths::transpose(modelMat));
        model.normalMatIndex = ++matIndex;
        mats[matIndex] = Mat34F(maths::inverse(camera.viewMat * modelMat));
    }
}

//==============================================================================

void DemoApp::populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf)
{
    cmdbuf.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

    // render models to multisample fb
    {
        cmdbuf.beginRenderPass (
            vk::RenderPassBeginInfo {
                mMsRenderPass.pass, mMsRenderPass.framebuf, vk::Rect2D({0, 0}, {mWindow->get_size().x, mWindow->get_size().y})
            }, vk::SubpassContents::eInline
        );

        cmdbuf.clearAttachments (
            vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0u, vk::ClearValue(std::array{0.025f, 0.025f, 0.f, 1.f})),
            vk::ClearRect(vk::Rect2D({0, 0}, {mWindow->get_size().x, mWindow->get_size().y}), 0u, 1u)
        );

        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 0u, mModelDescriptorSet.front, {});
        cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mModelPipelineLayout, 1u, mCaches->bindlessTextureSet, {});

        for (const auto& model : mStaticModels)
        {
            model.pipeline->bind(cmdbuf);
            model.mesh->bind_buffers(cmdbuf);

            struct {
                uint modelMatIndex, normalMatIndex;
                uint diffuseTexIndex, specularTexIndex;
                uint maskTexIndex, normalTexIndex;
            } pc;

            pc.modelMatIndex = model.modelMatIndex;
            pc.normalMatIndex = model.normalMatIndex;
            pc.diffuseTexIndex = *model.texDiffuse->get_bindless_descriptor_index();
            pc.specularTexIndex = *model.texSpecular->get_bindless_descriptor_index();
            if (model.texMask != nullptr)
                pc.maskTexIndex = *model.texMask->get_bindless_descriptor_index();
            if (model.texNormal != nullptr)
                pc.normalTexIndex = *model.texNormal->get_bindless_descriptor_index();

            cmdbuf.pushConstants (
                mPassConfigOpaque->pipelineLayout, vk::ShaderStageFlagBits::eAllGraphics,
                0u, sizeof(pc), &pc
            );

            model.mesh->draw(cmdbuf);
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
