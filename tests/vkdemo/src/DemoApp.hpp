#pragma once

#include "setup.hpp"

#include "Resources.hpp"

#include <sqee/app/Application.hpp>
#include <sqee/vk/VulkGuiSystem.hpp>
#include <sqee/vk/VulkInputDevices.hpp>
#include <sqee/vk/VulkMesh.hpp>
#include <sqee/vk/VulkTexture.hpp>
#include <sqee/vk/VulkWindow.hpp>
#include <sqee/vk/Pipeline.hpp>
#include <sqee/vk/VulkMaterial.hpp>
#include <sqee/vk/SwapBuffer.hpp>

namespace demo {

//============================================================================//

class DemoApp : public sq::Application
{
public: //====================================================//

    ~DemoApp() override;

private: //===================================================//

    void initialise(std::vector<String> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_graphics_config();

    //--------------------------------------------------------//

    void initialise_layouts();

    void initialise_camera();

    void initialise_models();

    //--------------------------------------------------------//

    void create_render_targets();

    void destroy_render_targets();

    void create_pipelines();

    void destroy_pipelines();

    //--------------------------------------------------------//

    void update_window_title(double elapsed);

    void update_uniform_buffer(double elapsed);

    void populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf);

    //--------------------------------------------------------//

    vk::SampleCountFlagBits mMultisampleMode = vk::SampleCountFlagBits::e4;

    std::unique_ptr<sq::VulkWindow> mWindow;
    std::unique_ptr<sq::VulkInputDevices> mInputDevices;
    std::unique_ptr<sq::VulkGuiSystem> mGuiSystem;

    ResourceCaches mResourceCaches;

    //--------------------------------------------------------//

    vk::DescriptorSetLayout mCameraDescriptorSetLayout;
    vk::DescriptorSetLayout mLightDescriptorSetLayout;
    vk::DescriptorSetLayout mModelDescriptorSetLayout;
    vk::PipelineLayout mModelPipelineLayout;

    vk::DescriptorSetLayout mCompositeDescriptorSetLayout;
    vk::PipelineLayout mCompositePipelineLayout;

    sq::Swapper<vk::DescriptorSet> mCameraDescriptorSet;
    sq::Swapper<vk::DescriptorSet> mLightDescriptorSet;
    vk::DescriptorSet mCompositeDescriptorSet;

    //--------------------------------------------------------//

    sq::SwapBuffer mCameraUbo;
    sq::SwapBuffer mLightUbo;

    struct StaticModel
    {
        MeshHandle mesh;
        MaterialHandle material;
        sq::SwapBuffer ubo;
        sq::Swapper<vk::DescriptorSet> descriptorSet;
    };

    std::vector<StaticModel> mStaticModels;

    //--------------------------------------------------------//

    vk::Image mMsColourImage;
    sq::VulkanMemory mMsColourImageMem;
    vk::ImageView mMsColourImageView;

    vk::Image mResolveColourImage;
    sq::VulkanMemory mResolveColourImageMem;
    vk::ImageView mResolveColourImageView;
    vk::Sampler mResolveColourSampler;

    vk::RenderPass mMsRenderPass;
    vk::Framebuffer mMsFramebuffer;

    //vk::Image mDepthImage;
    //sq::VulkanMemory mDepthImageMem;
    //vk::ImageView mDepthImageView;

    //--------------------------------------------------------//

    vk::Pipeline mCompositePipeline;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

//============================================================================//

} // namespace sqt
