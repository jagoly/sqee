#pragma once

#include "setup.hpp"

#include "Resources.hpp"

#include <sqee/app/Application.hpp>
#include <sqee/app/GuiSystem.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/Window.hpp>

#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

#include <sqee/vk/SwapBuffer.hpp>

namespace demo { //#############################################################

class DemoApp : public sq::Application
{
public: //======================================================

    ~DemoApp() override;

private: //=====================================================

    void initialise(std::vector<String> args) override;

    void update(double elapsed) override;

    //----------------------------------------------------------

    void handle_event(sq::Event event);

    void refresh_graphics_config();

    //----------------------------------------------------------

    void initialise_layouts();

    void initialise_camera();

    void initialise_models();

    //----------------------------------------------------------

    void create_render_targets();

    void destroy_render_targets();

    void create_pipelines();

    void destroy_pipelines();

    //----------------------------------------------------------

    void update_window_title(double elapsed);

    void update_uniform_buffer(double elapsed);

    void populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf);

    //----------------------------------------------------------

    vk::SampleCountFlagBits mMultisampleMode = vk::SampleCountFlagBits::e4;

    std::unique_ptr<sq::Window> mWindow;
    std::unique_ptr<sq::InputDevices> mInputDevices;
    std::unique_ptr<sq::GuiSystem> mGuiSystem;

    std::unique_ptr<ResourceCaches> mCaches;

    //----------------------------------------------------------

    vk::DescriptorSetLayout mModelDescriptorSetLayout;
    vk::PipelineLayout mModelPipelineLayout;

    vk::DescriptorSetLayout mCompositeDescriptorSetLayout;
    vk::PipelineLayout mCompositePipelineLayout;

    sq::Swapper<vk::DescriptorSet> mModelDescriptorSet;
    vk::DescriptorSet mCompositeDescriptorSet;

    sq::PassConfig* mPassConfigOpaque = nullptr;

    //----------------------------------------------------------

    sq::SwapBuffer mCameraUbo;
    sq::SwapBuffer mLightUbo;
    sq::SwapBuffer mMatrixUbo;

    // todo: replace with DrawItem
    struct StaticModel
    {
        MeshHandle mesh;
        PipelineHandle pipeline;
        TextureHandle texDiffuse;
        TextureHandle texSpecular;
        TextureHandle texMask;
        TextureHandle texNormal;
        uint modelMatIndex;
        uint normalMatIndex;
    };

    std::vector<StaticModel> mStaticModels;

    //----------------------------------------------------------

    sq::ImageStuff mMsColourImage;
    sq::ImageStuff mResolveColourImage;
    vk::Sampler mResolveColourSampler;
    sq::RenderPassStuff mMsRenderPass;

    //----------------------------------------------------------

    vk::Pipeline mCompositePipeline;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

} // namespace demo ############################################################
