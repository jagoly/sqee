#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/core/Types.hpp>

#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/VulkanMemory.hpp>

//====== Forward Declarations ================================================//

extern "C" { typedef struct GLFWwindow GLFWwindow; };

namespace sq {

//============================================================================//

class SQEE_API VulkWindow final : private NonCopyable
{
public: //====================================================//

    VulkWindow(const char* title, Vec2U size, const char* appName, Vec3U version);

    ~VulkWindow();

    //--------------------------------------------------------//

    void create_swapchain_and_friends();

    void destroy_swapchain_and_friends();

    //--------------------------------------------------------//

    const std::vector<Event>& fetch_events();

    std::tuple<vk::CommandBuffer, vk::Framebuffer> begin_frame();

    void submit_present_swap();

    //--------------------------------------------------------//

    GLFWwindow* get_glfw_window() { return mGlfwWindow; }

    vk::RenderPass get_render_pass() const { return mRenderPass; }

    Vec2U get_size() const { return { mFramebufferSize.width, mFramebufferSize.height }; }

    //--------------------------------------------------------//

    void set_title(String title);

    void set_cursor_hidden(bool hidden);

    void set_vsync_enabled(bool enabled);

    void set_key_repeat(bool /*repeat*/) {} // todo

    //--------------------------------------------------------//

    const String& get_title() const { return mTitle; };

    bool get_cursor_hidden() const { return mCursorHidden; };

    bool get_vsync_enabled() const { return mVsyncEnabled; };

    //--------------------------------------------------------//

    bool has_focus() const;

private: //===================================================//

    GLFWwindow* mGlfwWindow = nullptr;

    vk::Extent2D mFramebufferSize = {};
    uint32_t mImageIndex = 0u;

    VulkanAllocator mAllocator;

    std::vector<Event> mEvents, mEventsOld;

    //--------------------------------------------------------//

    vk::Instance mInstance;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
    vk::SurfaceKHR mSurface;
    vk::PhysicalDevice mPhysicalDevice;
    vk::Device mDevice;
    vk::Queue mQueue;

    vk::CommandPool mCommandPool;
    vk::DescriptorPool mDesciptorPool;

    vk::RenderPass mRenderPass;

    vk::SwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;    
    std::vector<vk::ImageView> mSwapchainImageViews;
    std::vector<vk::Framebuffer> mSwapchainFramebuffers;

    Swapper<vk::CommandBuffer> mCommandBuffer;
    Swapper<vk::Semaphore> mImageAvailableSemaphore;
    Swapper<vk::Semaphore> mRenderFinishedSemaphore;
    Swapper<vk::Fence> mRenderFinishedFence;

    //--------------------------------------------------------//

    String mTitle;
    bool mCursorHidden = false;
    bool mVsyncEnabled = false;

    //--------------------------------------------------------//

    struct Implementation;
    friend Implementation;
};

} // namesapce sq
