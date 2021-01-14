#include "VulkTestApp.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

void VulkTestApp::initialise(std::vector<std::string> /*args*/)
{
    mWindow = std::make_unique<sq::VulkWindow>("Hello Vulkan", Vec2U(800u, 600u), "sqee-vulkan", 0, 0, 1);

    create_demo_pipeline();

    populate_command_buffer();
}

//============================================================================//

VulkTestApp::~VulkTestApp()
{
    vk::Device device = mWindow->get_device();

    device.waitIdle();

    mWindow->free_command_buffers();

    device.destroy(mPipeline);
    device.destroy(mPipelineLayout);
}

//============================================================================//

void VulkTestApp::update(double elapsed)
{
    for (auto& event : mWindow->fetch_events())
    {
        if (event.type == sq::Event::Type::Window_Close)
            mReturnCode = 0;

        if (event.type == sq::Event::Type::Window_Resize)
        {
            mWindow->handle_window_resize();
            handle_window_resize();
        }
    }

    render(elapsed);

    mWindow->submit_and_present();
}

//============================================================================//

void VulkTestApp::create_demo_pipeline()
{
    auto device = mWindow->get_device();
    auto framebufferSize = mWindow->get_framebuffer_size();

    // create pipeline layout
    {
        mPipelineLayout = device.createPipelineLayout (
            vk::PipelineLayoutCreateInfo { {}, {}, {} }
        );
    }

    // load shaders and create graphics pipeline
    {
        auto vertShaderCode = sq::get_bytes_from_file("shaders/screen.vert.spv");
        auto fragShaderCode = sq::get_bytes_from_file("shaders/screen.frag.spv");

        auto vertShaderModule = device.createShaderModule (
            vk::ShaderModuleCreateInfo {
                {}, vertShaderCode.size(), reinterpret_cast<uint32_t*>(vertShaderCode.data())
            }
        );

        auto fragShaderModule = device.createShaderModule (
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

        auto vertexInputState = vk::PipelineVertexInputStateCreateInfo {
            {}, {}, {}
        };

        auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

        auto viewports = vk::Viewport { 0.f, 0.f, float(framebufferSize.x), float(framebufferSize.y), 0.f, 1.f };
        auto scissors = vk::Rect2D { { 0, 0 }, { framebufferSize.x, framebufferSize.y } };

        auto viewportState = vk::PipelineViewportStateCreateInfo {
            {}, viewports, scissors
        };

        auto rasterizationState = vk::PipelineRasterizationStateCreateInfo {
            {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
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

        auto result = device.createGraphicsPipeline (
            nullptr,
            vk::GraphicsPipelineCreateInfo {
                {}, stages, &vertexInputState, &inputAssemblyState, nullptr,
                &viewportState, &rasterizationState, &multisampleState, nullptr, &colorBlendState,
                nullptr, mPipelineLayout, mWindow->get_renderpass(), 0u
            }
        );
        SQASSERT(result.result == vk::Result::eSuccess, "");

        mPipeline = result.value;

        device.destroyShaderModule(vertShaderModule);
        device.destroyShaderModule(fragShaderModule);
    }
}

//============================================================================//

void VulkTestApp::populate_command_buffer()
{
    auto framebufferSize = mWindow->get_framebuffer_size();

    for (size_t i = 0u; i < mWindow->get_swapchain_image_count(); ++i)
    {
        auto& commandBuffer = mWindow->get_commandbuffers()[i];

        commandBuffer.begin (
            vk::CommandBufferBeginInfo { {}, nullptr }
        );

        auto clearValues = vk::ClearValue { std::array<float,4> { 0.1f, 0.1f, 0.f, 1.f } };

        commandBuffer.beginRenderPass (
            vk::RenderPassBeginInfo {
                mWindow->get_renderpass(), mWindow->get_framebuffers()[i],
                vk::Rect2D { { 0, 0 }, { framebufferSize.x, framebufferSize.y } },
                clearValues
            },
            vk::SubpassContents::eInline
        );

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

        for (uint i = 0u; i < 200u; ++i)
            commandBuffer.draw(3u, 1u, 0u, 0u);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }
}

//============================================================================//

void VulkTestApp::render(double elapsed)
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
    auto device = mWindow->get_device();

    device.destroy(mPipeline);
    device.destroy(mPipelineLayout);

    create_demo_pipeline();

    populate_command_buffer();
}
