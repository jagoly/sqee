#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/core/Utilities.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>

//==============================================================================

extern "C" { typedef struct GLFWwindow GLFWwindow; }

namespace sq { //###############################################################

enum class CursorMode : int
{
    Normal   = 0x00034001, ///< visible, able to leave window
    Hidden   = 0x00034002, ///< hidden, able to leave window
    Disabled = 0x00034003, ///< hidden, locked to window, unrestricted range
    Captured = 0x00034004, ///< visible, locked to window, clamp to window bounds
};

class SQEE_API Window
{
public: //======================================================

    Window(const char* title, Vec2U size, const char* appName, Vec3U version);

    SQEE_COPY_DELETE(Window)
    SQEE_MOVE_DELETE(Window)

    ~Window();

    //----------------------------------------------------------

    void set_size_limits(std::optional<Vec2U> minimum, std::optional<Vec2U> maximum);

    //----------------------------------------------------------

    void create_swapchain_and_friends();

    void destroy_swapchain_and_friends();

    //----------------------------------------------------------

    const std::vector<Event>& fetch_events();

    void begin_frame();

    std::tuple<vk::CommandBuffer, vk::Framebuffer> acquire_image();

    void submit_present_swap();

    //----------------------------------------------------------

    GLFWwindow* get_glfw_window() { return mGlfwWindow; }

    vk::RenderPass get_render_pass() const { return mRenderPass; }

    Vec2U get_size() const { return { mFramebufferSize.width, mFramebufferSize.height }; }

    //----------------------------------------------------------

    void set_title(const String& title);

    const String get_title() const;

    void set_cursor_mode(CursorMode mode);

    CursorMode get_cursor_mode() const;

    void set_vsync_enabled(bool enabled);

    bool get_vsync_enabled() const { return mVsyncEnabled; };

    //----------------------------------------------------------

    bool has_focus() const;

private: //=====================================================

    GLFWwindow* mGlfwWindow = nullptr;

    vk::Extent2D mFramebufferSize = {};
    uint32_t mImageIndex = 0u;

    VulkanAllocator mAllocator;

    std::vector<Event> mEvents, mEventsOld;

    //----------------------------------------------------------

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

    //----------------------------------------------------------

    bool mVsyncEnabled = false;

    bool mPendingResize = true;

    //----------------------------------------------------------

    struct Implementation;
    friend Implementation;
};

} // namesapce sq ##############################################################
