#pragma once

#include <sqee/app/Application.hpp>

#include <sqee/vk/VulkWindow.hpp>
#include <sqee/vk/VulkInputDevices.hpp>

#include <sqee/vk/VulkTexture.hpp>
#include <sqee/vk/VulkMesh.hpp>

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
    std::unique_ptr<sq::VulkInputDevices> mInputDevices;

//    vk::Buffer mVertexBuffer;
//    sq::VulkanMemory mVertexBufferMem;
//    vk::Buffer mIndexBuffer;
//    sq::VulkanMemory mIndexBufferMem;

    sq::VulkMesh mMesh;

//    vk::Image mTextureImage;
//    sq::VulkanMemory mTextureImageMem;
//    vk::ImageView mTextureImageView;
//    vk::Sampler mTextureSampler;

    sq::VulkTexture mTexture;

    sq::Swapper<vk::Buffer> mCameraUbo;
    sq::Swapper<sq::VulkanMemory> mCameraUboMem;
    sq::Swapper<void*> mCameraUboMemPtr;

    vk::DescriptorSetLayout mDescriptorSetLayout;
    sq::Swapper<vk::DescriptorSet> mDescriptorSet;

    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mPipeline;

    double mTimeAccum = 0.0;
    uint mFramesAccum = 0u;
};

} // namespace sqt
