#pragma once

#include <sqee/app/Application.hpp>

#include <sqee/vk/VulkWindow.hpp>

using namespace sq::coretypes;

namespace sqt {

//============================================================================//

class VulkTestApp : public sq::Application
{

public: //====================================================//

    ~VulkTestApp();

private: //===================================================//

    void initialise(std::vector<std::string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void create_buffers();

    void create_textures();

    void create_descriptor_sets();

    void create_demo_pipeline();

    //--------------------------------------------------------//

    void update_uniform_buffer(double elapsed);

    void populate_command_buffer(double elapsed);

    void update_window_title(double elapsed);

    void handle_window_resize();

    //--------------------------------------------------------//

    std::unique_ptr<sq::VulkWindow> mWindow;

    vk::Buffer mVertexBuffer;
    vk::DeviceMemory mVertexBufferMem;
    vk::Buffer mIndexBuffer;
    vk::DeviceMemory mIndexBufferMem;

    vk::Image mTextureImage;
    vk::DeviceMemory mTextureImageMem;
    vk::ImageView mTextureImageView;
    vk::Sampler mTextureSampler;

    sq::Swapper<vk::Buffer> mCameraUbo;
    sq::Swapper<vk::DeviceMemory> mCameraUboMem;
    sq::Swapper<void*> mCameraUboMemPtr;

    vk::DescriptorSetLayout mDescriptorSetLayout;
    sq::Swapper<vk::DescriptorSet> mDescriptorSet;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

} // namespace sqt
