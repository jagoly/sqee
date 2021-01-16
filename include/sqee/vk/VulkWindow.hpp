#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//====== Forward Declarations ================================================//

struct Event;
struct VulkContext;

//============================================================================//

struct SQEE_API VulkWindow
{
    VulkWindow(const char* title, Vec2U size, const char* appName, int vMajor, int vMinor, int vPatch);

    ~VulkWindow();

    //--------------------------------------------------------//

    void create_swapchain_and_friends();

    void destroy_swapchain_and_friends();

    void handle_window_resize();

    //--------------------------------------------------------//

    const std::vector<Event>& fetch_events();

    void begin_new_frame();

    void submit_and_present();

    //--------------------------------------------------------//

    VulkContext get_context() const;

    //--------------------------------------------------------//

    void set_title(String title);

    void set_vsync_enabled(bool enabled);

private: //===================================================//

    void* mGlfwWindow = nullptr;

    uint mMemoryTypeIndexHost = 0u;
    uint mMemoryTypeIndexDevice = 0u;

    float mMaxAnisotropy = 0.f;

    Vec2U mFramebufferSize = {};
    uint mImageIndex = 0u;

    bool mNeedWaitFenceOther = false;

    //--------------------------------------------------------//

    vk::Instance mInstance;
    vk::SurfaceKHR mSurface;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mQueue;

    vk::CommandPool mCommandPool;
    vk::DescriptorPool mDesciptorPool;

    vk::SwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;
    std::vector<vk::ImageView> mSwapchainImageViews;
    std::vector<vk::Framebuffer> mSwapchainFramebuffers;

    sq::Swapper<vk::CommandBuffer> mCommandBuffer;
    sq::Swapper<vk::Semaphore> mImageAvailableSemaphore;
    sq::Swapper<vk::Semaphore> mRenderFinishedSemaphore;
    sq::Swapper<vk::Fence> mRenderFinishedFence;

    vk::RenderPass mRenderPass;

    //--------------------------------------------------------//

    struct Implementation;
    std::unique_ptr<Implementation> impl;

    friend Implementation;
};

//============================================================================//

} // namesapce sq
