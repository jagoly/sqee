#include "VulkTestApp.hpp"

#include "UniformBlocks.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/redist/stb_image.hpp>

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

struct Vertex
{
    Vec2F position; Vec3F colour; Vec2F texcoord;

    static vk::VertexInputBindingDescription get_binding_description()
    {
        return vk::VertexInputBindingDescription {
            0u, sizeof(Vertex), vk::VertexInputRate::eVertex
        };
    }

    static std::array<vk::VertexInputAttributeDescription, 3> get_attribute_description()
    {
        return std::array {
            vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position) },
            vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, colour) },
            vk::VertexInputAttributeDescription { 2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texcoord) }
        };
    }
};

const std::vector<Vertex> g_vertices =
{
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> g_indices = { 0, 1, 2, 2, 3, 0 };

//============================================================================//

void VulkTestApp::initialise(std::vector<std::string> /*args*/)
{
    mWindow = std::make_unique<sq::VulkWindow>("Hello Vulkan", Vec2U(800u, 600u), "sqee-vulkan", 0, 0, 1);

    create_buffers();

    create_textures();

    create_descriptor_sets();

    create_demo_pipeline();
}

//============================================================================//

VulkTestApp::~VulkTestApp()
{
    const auto ctx = mWindow->get_context();

    ctx.device.waitIdle();

    ctx.device.destroy(mTextureSampler);
    ctx.device.destroy(mTextureImageView);
    ctx.device.destroy(mTextureImage);
    ctx.device.free(mTextureImageMem);

    ctx.device.destroy(mVertexBuffer);
    ctx.device.free(mVertexBufferMem);

    ctx.device.destroy(mIndexBuffer);
    ctx.device.free(mIndexBufferMem);

    ctx.device.destroy(mCameraUbo.front);
    ctx.device.destroy(mCameraUbo.back);
    ctx.device.free(mCameraUboMem.front);
    ctx.device.free(mCameraUboMem.back);

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
        {
            mWindow->handle_window_resize();
            handle_window_resize();
        }
    }

    mWindow->begin_new_frame();

    update_uniform_buffer(elapsed);

    populate_command_buffer(elapsed);

    update_window_title(elapsed);

    mWindow->submit_and_present();

    mCameraUbo.swap();
    mCameraUboMem.swap();
    mCameraUboMemPtr.swap();
    mDescriptorSet.swap();
}

//============================================================================//

void VulkTestApp::create_buffers()
{
    const auto ctx = mWindow->get_context();

    // vertex buffer
    {
        const size_t bufferSize = sizeof(Vertex) * g_vertices.size();

        auto [stagingBuffer, stagingBufferMem] = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, true
        );

        std::memcpy(ctx.device.mapMemory(stagingBufferMem, 0u, bufferSize, {}), g_vertices.data(), bufferSize);
        ctx.device.unmapMemory(stagingBufferMem);

        std::tie(mVertexBuffer, mVertexBufferMem) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, false
        );

        sq::vk_copy_buffer(ctx, stagingBuffer, mVertexBuffer, bufferSize);

        ctx.device.destroy(stagingBuffer);
        ctx.device.free(stagingBufferMem);
    }

    // index buffer
    {
        const size_t bufferSize = sizeof(uint16_t) * g_indices.size();

        auto [stagingBuffer, stagingBufferMem] = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, true
        );

        std::memcpy(ctx.device.mapMemory(stagingBufferMem, 0u, bufferSize, {}), g_indices.data(), bufferSize);
        ctx.device.unmapMemory(stagingBufferMem);

        std::tie(mIndexBuffer, mIndexBufferMem) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, false
        );

        sq::vk_copy_buffer(ctx, stagingBuffer, mIndexBuffer, bufferSize);

        ctx.device.destroy(stagingBuffer);
        ctx.device.free(stagingBufferMem);
    }

    // camera uniform buffer
    {
        const size_t bufferSize = sizeof(CameraBlock);

        std::tie(mCameraUbo.front, mCameraUboMem.front) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true
        );
        mCameraUboMemPtr.front = ctx.device.mapMemory(mCameraUboMem.front, 0u, bufferSize, {});

        std::tie(mCameraUbo.back, mCameraUboMem.back) = sq::vk_create_buffer (
            ctx, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true
        );
        mCameraUboMemPtr.back = ctx.device.mapMemory(mCameraUboMem.back, 0u, bufferSize, {});
    }
}

//============================================================================//

void VulkTestApp::create_textures()
{
    const auto ctx = mWindow->get_context();

    // create and load image
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(0);
        stbi_uc* pixels = stbi_load("assets/textures/SQEE.png", &width, &height, &channels, 4);

        size_t imageSize = size_t(width * height * 4);

        auto [stagingBuffer, stagingBufferMem] = sq::vk_create_buffer (
            ctx, imageSize, vk::BufferUsageFlagBits::eTransferSrc, true
        );

        std::memcpy(ctx.device.mapMemory(stagingBufferMem, 0u, imageSize, {}), pixels, imageSize);
        ctx.device.unmapMemory(stagingBufferMem);

        stbi_image_free(pixels);

        std::tie(mTextureImage, mTextureImageMem) = sq::vk_create_image (
            ctx, Vec2U(width, height), vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, false
        );

        sq::vk_transfer_buffer_to_image(ctx, stagingBuffer, mTextureImage, Vec2U(width, height));

        ctx.device.destroy(stagingBuffer);
        ctx.device.free(stagingBufferMem);
    }

    // create image view and sampler
    {
        mTextureImageView = ctx.device.createImageView (
            vk::ImageViewCreateInfo {
                {}, mTextureImage, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb, vk::ComponentMapping(),
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
            }
        );

        mTextureSampler = ctx.device.createSampler (
            vk::SamplerCreateInfo {
                {}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
                vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
                0.f, true, ctx.maxAnisotropy, false, vk::CompareOp::eAlways, 0.f, 0.f,
                vk::BorderColor::eIntOpaqueWhite, false
            }
        );
    }
}

//============================================================================//

void VulkTestApp::create_descriptor_sets()
{
    const auto ctx = mWindow->get_context();

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
            vk::DescriptorImageInfo { mTextureSampler, mTextureImageView, vk::ImageLayout::eShaderReadOnlyOptimal }
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
    const auto ctx = mWindow->get_context();

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

        auto vertexBindingDescriptions = Vertex::get_binding_description();
        auto vertexAttributeDescriptions = Vertex::get_attribute_description();

        auto vertexInputState = vk::PipelineVertexInputStateCreateInfo {
            {}, vertexBindingDescriptions, vertexAttributeDescriptions
        };

        auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

        //auto viewports = vk::Viewport { 0.f, 0.f, float(ctx.windowSize.x), float(ctx.windowSize.y), 0.f, 1.f };
        //auto scissors = vk::Rect2D { { 0, 0 }, { ctx.windowSize.x, ctx.windowSize.y } };

        auto viewportState = vk::PipelineViewportStateCreateInfo {
            {}, 1u, nullptr, 1u, nullptr //viewports, scissors
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
    const auto ctx = mWindow->get_context();

    static double totalTime = 0.0;
    totalTime += elapsed * 0.1;

    auto& block = *static_cast<CameraBlock*>(mCameraUboMemPtr.front);

    block.model = Mat4F(maths::rotation(Vec3F(0.f, 1.f, 0.f), float(totalTime)));
    block.view = maths::look_at_RH(Vec3F(0.f, 0.1f, 2.f), Vec3F(0.f, 0.f, 0.f), Vec3F(0.f, 1.f, 0.f));

    const float aspect = float(ctx.windowSize.x) / float(ctx.windowSize.y);
    block.proj = maths::perspective_RH(maths::radians(0.125f), aspect, 0.1f, 10.f);
}

//============================================================================//

void VulkTestApp::populate_command_buffer(double /*elapsed*/)
{
    const auto ctx = mWindow->get_context();
    const auto cmdbuf = ctx.commandBuffer;

    cmdbuf.begin({{}, nullptr});

    auto clearValues = vk::ClearValue { std::array<float,4> { 0.1f, 0.1f, 0.f, 1.f } };

    cmdbuf.beginRenderPass (
        vk::RenderPassBeginInfo {
            ctx.renderPass, ctx.framebuffer,
            vk::Rect2D { { 0, 0 }, { ctx.windowSize.x, ctx.windowSize.y } },
            clearValues
        },
        vk::SubpassContents::eInline
    );

    cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

    cmdbuf.setViewport(0u, vk::Viewport{0.f, 0.f, float(ctx.windowSize.x), float(ctx.windowSize.y), 0.f, 1.f});
    cmdbuf.setScissor(0u, vk::Rect2D{{ 0, 0 }, { ctx.windowSize.x, ctx.windowSize.y }});

    cmdbuf.bindVertexBuffers(0u, mVertexBuffer, size_t(0u));
    cmdbuf.bindIndexBuffer(mIndexBuffer, 0u, vk::IndexType::eUint16);

    cmdbuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0u, mDescriptorSet.front, {});

    //for (uint i = 0u; i < 200u; ++i)
        cmdbuf.drawIndexed(uint(g_indices.size()), 1u, 0u, 0, 0u);

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
    //const auto ctx = mWindow->get_context();

    //ctx.device.destroy(mPipeline);
    //ctx.device.destroy(mPipelineLayout);
}
