#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

//namespace sqt { class VulkTestApp; }

namespace sq {

//====== Forward Declarations ================================================//

class InputDevices;
struct Event;

struct SQEE_API VulkWindow
{
//    friend sqt::VulkTestApp;

    VulkWindow(const char* title, Vec2U size, const char* appName, int vMajor, int vMinor, int vPatch);

    ~VulkWindow();

    //--------------------------------------------------------//

    void create_swapchain_and_friends();

    void destroy_swapchain_and_friends();

    void allocate_command_buffers();

    void free_command_buffers();

    const std::vector<Event>& fetch_events();

    void handle_window_resize();

    void submit_and_present();

    //--------------------------------------------------------//

    const vk::Device& get_device() const;

    const vk::RenderPass& get_renderpass() const;

    const std::vector<vk::Framebuffer>& get_framebuffers() const;

    const std::vector<vk::CommandBuffer>& get_commandbuffers() const;

    uint get_swapchain_image_count() const;

    Vec2U get_framebuffer_size() const;

    //bool get_window_should_close() const;

    //--------------------------------------------------------//

    void set_title(String title);

    void set_vsync_enabled(bool enabled);

    void swap_buffers();

private: //===================================================//

    void* mGlfwWindow = nullptr;

    uint mPrevImageIndex = 0u;

    Vec2U mFramebufferSize;

    bool mNeedWaitFenceOther = false;

    //--------------------------------------------------------//

    vk::Instance mInstance;
    vk::SurfaceKHR mSurface;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mQueue;

    vk::SwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;
    std::vector<vk::ImageView> mSwapchainImageViews;
    std::vector<vk::Framebuffer> mSwapchainFramebuffers;

    vk::Semaphore mImageAvailableSemaphore, mImageAvailableSemaphoreOther;
    vk::Semaphore mRenderFinishedSemaphore, mRenderFinishedSemaphoreOther;
    vk::Fence mRenderFinishedFence, mRenderFinishedFenceOther;
    vk::Fence mImageBusyFence, mImageBusyFenceOther;

    vk::CommandPool mCommandPool;
    std::vector<vk::CommandBuffer> mCommandBuffers;

    vk::RenderPass mRenderPass;

    //--------------------------------------------------------//

    struct Implementation;
    std::unique_ptr<Implementation> impl;

    friend Implementation;
};

} // namesapce sq
