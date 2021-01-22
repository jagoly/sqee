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

void VulkTestApp::initialise(std::vector<std::string> /*args*/)
{
    mWindow = std::make_unique<sq::VulkWindow> (
        "Hello Vulkan", Vec2U(800u, 600u), "sqee-vkdemo", Vec3U(0u, 0u, 1u)
    );

    mInputDevices = std::make_unique<sq::VulkInputDevices>(*mWindow);

    create_buffers();
    create_textures();
    create_descriptor_sets();
    create_demo_pipeline();
}

//============================================================================//

VulkTestApp::~VulkTestApp()
{
    const auto& ctx = mWindow->get_context();

    ctx.device.waitIdle();

    ctx.device.destroy(mCameraUbo.front);
    mCameraUboMem.front.free();
    ctx.device.destroy(mCameraUbo.back);
    mCameraUboMem.back.free();

    ctx.device.destroy(mDescriptorSetLayout);

    ctx.device.destroy(mPipeline);
    ctx.device.destroy(mPipelineLayout);
}

//============================================================================//

void VulkTestApp::update(double elapsed)
{
    for (auto& event : mWindow->fetch_events())
    {
        if (event.type == sq::Event::Type::Window_Close)
            mReturnCode = 0;

        if (event.type == sq::Event::Type::Window_Resize)
            handle_window_resize();
    }

    if (mWindow->begin_new_frame() == true)
    {
        update_uniform_buffer(elapsed);
        populate_command_buffer(elapsed);
        update_window_title(elapsed);

        mWindow->submit_and_present();

        mCameraUbo.swap();
        mCameraUboMem.swap();
        mCameraUboMemPtr.swap();
        mDescriptorSet.swap();
    }
}

//============================================================================//

void VulkTestApp::create_buffers()
{
    const auto& ctx = mWindow->get_context();

    mMesh.load_from_file(ctx, "assets/meshes/Dice");

    // camera uniform buffer
    {
        const size_t bufferSize = sizeof(CameraBlock);

        std::tie(mCameraUbo.front, mCameraUboMem.front) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true
        );
        mCameraUboMemPtr.front = mCameraUboMem.front.map();

        std::tie(mCameraUbo.back, mCameraUboMem.back) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true
        );
        mCameraUboMemPtr.back = mCameraUboMem.back.map();
    }
}

//============================================================================//

void VulkTestApp::create_textures()
{
    const auto& ctx = mWindow->get_context();

    mTexture.load_automatic_2D(ctx, "assets/textures/Dice_diff");
}

//============================================================================//

void VulkTestApp::create_descriptor_sets()
{
    const auto& ctx = mWindow->get_context();

    auto bindings = std::array {
        vk::DescriptorSetLayoutBinding {
            0u, vk::DescriptorType::eUniformBuffer, 1u, vk::ShaderStageFlagBits::eVertex, nullptr
        },
        vk::DescriptorSetLayoutBinding {
            1u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment, nullptr
        }
    };

    mDescriptorSetLayout = ctx.device.createDescriptorSetLayout (
        vk::DescriptorSetLayoutCreateInfo { {}, bindings }
    );

    auto create_descriptor_set = [&](vk::Buffer cameraUbo)
    {
        auto descriptorSet = ctx.device.allocateDescriptorSets (
            vk::DescriptorSetAllocateInfo { ctx.descriptorPool, mDescriptorSetLayout }
        ).front();

        auto bufferInfo = std::array {
            vk::DescriptorBufferInfo { cameraUbo, 0u, sizeof(CameraBlock) }
        };

        auto imageInfo = std::array {
            mTexture.get_descriptor_info()
        };

        auto descriptorWrites = std::array {
            vk::WriteDescriptorSet {
                descriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer, {}, bufferInfo, {}
            },
            vk::WriteDescriptorSet {
                descriptorSet, 1u, 0u, vk::DescriptorType::eCombinedImageSampler, imageInfo, {}, {}
            }
        };

        ctx.device.updateDescriptorSets(descriptorWrites, {});

        return descriptorSet;
    };

    mDescriptorSet.front = create_descriptor_set(mCameraUbo.front);
    mDescriptorSet.back = create_descriptor_set(mCameraUbo.back);
}

//============================================================================//

void VulkTestApp::create_demo_pipeline()
{
    const auto& ctx = mWindow->get_context();

    // create pipeline layout
    {
        mPipelineLayout = ctx.device.createPipelineLayout (
            vk::PipelineLayoutCreateInfo { {}, mDescriptorSetLayout, {} }
        );
    }

    // load shaders and create graphics pipeline
    {
        auto vertShaderCode = sq::get_bytes_from_file("shaders/screen.vert.spv");
        auto fragShaderCode = sq::get_bytes_from_file("shaders/screen.frag.spv");

        auto vertShaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo {
                {}, vertShaderCode.size(), reinterpret_cast<uint32_t*>(vertShaderCode.data())
            }
        );

        auto fragShaderModule = ctx.device.createShaderModule (
            vk::ShaderModuleCreateInfo {
                {}, fragShaderCode.size(), reinterpret_cast<uint32_t*>(fragShaderCode.data())
            }
        );

        auto stages = std::array {
            vk::PipelineShaderStageCreateInfo {
                {}, vk::ShaderStageFlagBits::eVertex, vertShaderModule, "main", nullptr
            },
            vk::PipelineShaderStageCreateInfo {
                {}, vk::ShaderStageFlagBits::eFragment, fragShaderModule, "main", nullptr
            }
        };

        auto vertexInputState = mMesh.get_vertex_input_state_info();

        auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

        auto viewportState = vk::PipelineViewportStateCreateInfo {
            {}, 1u, nullptr, 1u, nullptr //viewports, scissors
        };

        auto rasterizationState = vk::PipelineRasterizationStateCreateInfo {
            {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
            vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
        };

        auto multisampleState = vk::PipelineMultisampleStateCreateInfo {
            {}, vk::SampleCountFlagBits::e1, false, 1.f, nullptr, false, false
        };

        auto colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        auto attachments = vk::PipelineColorBlendAttachmentState {
            false, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, colorWriteMask
        };

        auto colorBlendState = vk::PipelineColorBlendStateCreateInfo {
            {}, false, vk::LogicOp::eClear, attachments, { 0.f, 0.f, 0.f, 0.f }
        };

        auto states = std::array { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        auto dynamicState = vk::PipelineDynamicStateCreateInfo { {}, states };

        auto result = ctx.device.createGraphicsPipeline (
            nullptr,
            vk::GraphicsPipelineCreateInfo {
                {}, stages, &vertexInputState, &inputAssemblyState, nullptr,
                &viewportState, &rasterizationState, &multisampleState, nullptr, &colorBlendState,
                &dynamicState, mPipelineLayout, ctx.renderPass, 0u
                //nullptr, mPipelineLayout, ctx.renderPass, 0u
            }
        );
        SQASSERT(result.result == vk::Result::eSuccess, "");

        mPipeline = result.value;

        ctx.device.destroyShaderModule(vertShaderModule);
        ctx.device.destroyShaderModule(fragShaderModule);
    }
}

//============================================================================//

void VulkTestApp::update_uniform_buffer(double elapsed)
{
    const auto& ctx = mWindow->get_context();

    static double totalTime = 0.0;
    totalTime += elapsed * 0.2;

    auto& block = *static_cast<CameraBlock*>(mCameraUboMemPtr.front);

    static Mat4F model = Mat4F();

    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Up))
        model = Mat4F(maths::rotation(Vec3F(1.f, 0.f, 0.f), float(elapsed))) * model;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Down))
        model = Mat4F(maths::rotation(Vec3F(-1.f, 0.f, 0.f), float(elapsed))) * model;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Left))
        model = Mat4F(maths::rotation(Vec3F(0.f, 1.f, 0.f), float(elapsed))) * model;
    if (mInputDevices->is_pressed(sq::Keyboard_Key::Arrow_Right))
        model = Mat4F(maths::rotation(Vec3F(0.f, -1.f, 0.f), float(elapsed))) * model;

    block.model = model;
    block.view = maths::look_at_RH(Vec3F(0.f, 0.1f, -2.f), Vec3F(0.f, 0.f, 0.f), Vec3F(0.f, 1.f, 0.f));

    const float aspect = float(ctx.windowSize.x) / float(ctx.windowSize.y);
    block.proj = maths::perspective_RH(maths::radians(0.125f), aspect, 0.1f, 10.f);
}

//============================================================================//

void VulkTestApp::populate_command_buffer(double /*elapsed*/)
{
    const auto& ctx = mWindow->get_context();
    const auto& cmdbuf = ctx.frame.commandBuffer;

    cmdbuf.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

    auto clearValues = vk::ClearValue { std::array<float,4> { 0.1f, 0.1f, 0.f, 1.f } };

    cmdbuf.beginRenderPass (
        vk::RenderPassBeginInfo {
            ctx.renderPass, ctx.frame.framebuffer,
            vk::Rect2D { { 0, 0 }, { ctx.windowSize.x, ctx.windowSize.y } },
            clearValues
        },
        vk::SubpassContents::eInline
    );

    cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

    cmdbuf.setViewport(0u, vk::Viewport{0.f, 0.f, float(ctx.windowSize.x), float(ctx.windowSize.y), 0.f, 1.f});
    cmdbuf.setScissor(0u, vk::Rect2D{{ 0, 0 }, { ctx.windowSize.x, ctx.windowSize.y }});

    mMesh.bind_buffers(cmdbuf);

    cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0u, mDescriptorSet.front, {});

    //for (uint i = 0u; i < 200u; ++i)
        mMesh.draw_complete(cmdbuf);

    cmdbuf.endRenderPass();
    cmdbuf.end();
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

void VulkTestApp::handle_window_resize()
{
    const auto& ctx = mWindow->get_context();

    ctx.device.waitIdle();
    mWindow->destroy_swapchain_and_friends();
    mWindow->create_swapchain_and_friends();
}
