#pragma once

#include <sqee/app/Application.hpp>

#include <sqee/vk/VulkWindow.hpp>
#include <sqee/vk/VulkInputDevices.hpp>
#include <sqee/vk/VulkGuiSystem.hpp>

#include <sqee/vk/VulkTexture.hpp>
#include <sqee/vk/VulkMesh.hpp>

namespace sqt {

using namespace sq::coretypes;

//============================================================================//

class VulkTestApp : public sq::Application
{

public: //====================================================//

    ~VulkTestApp();

private: //===================================================//

    void initialise(std::vector<String> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void swap_resources();

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

    //--------------------------------------------------------//

    vk::DescriptorSetLayout mCameraDescriptorSetLayout;
    vk::DescriptorSetLayout mModelDescriptorSetLayout;
    vk::DescriptorSetLayout mCompositeDescriptorSetLayout;

    //--------------------------------------------------------//

    sq::SwapBuffer mCameraUbo;
    sq::Swapper<vk::DescriptorSet> mCameraDescriptorSet;

    struct Model
    {
        sq::VulkMesh mesh;
        sq::VulkTexture texture;
        sq::SwapBuffer ubo;
        sq::Swapper<vk::DescriptorSet> descriptorSet;
    };

    std::vector<Model> mModels;

    //--------------------------------------------------------//

    vk::Image mMsColourImage;
    sq::VulkanMemory mMsColourImageMem;
    vk::ImageView mMsColourImageView;

    vk::RenderPass mMsRenderPass;
    vk::Framebuffer mMsFramebuffer;

    vk::Image mResolveColourImage;
    sq::VulkanMemory mResolveColourImageMem;
    vk::ImageView mResolveColourImageView;
    vk::Sampler mResolveColourSampler;

    //vk::Image mDepthImage;
    //sq::VulkanMemory mDepthImageMem;
    //vk::ImageView mDepthImageView;

    //--------------------------------------------------------//

    vk::PipelineLayout mModelPipelineLayout;
    vk::Pipeline mModelPipeline;

    vk::PipelineLayout mCompositePipelineLayout;
    vk::Pipeline mCompositePipeline;

    vk::DescriptorSet mCompositeDescriptorSet;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

//============================================================================//

} // namespace sqt
