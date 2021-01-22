#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

//====== Forward Declarations ================================================//

extern "C" { typedef struct GLFWwindow GLFWwindow; };

namespace sq {

struct Event;

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

    bool begin_new_frame();

    void submit_and_present();

    //--------------------------------------------------------//

    GLFWwindow* get_glfw_window() { return mGlfwWindow; }

    const VulkanContext& get_context() const { return mContext; }

    //--------------------------------------------------------//

    void set_title(String title);

    void set_vsync_enabled(bool enabled);

private: //===================================================//

    GLFWwindow* mGlfwWindow = nullptr;

    vk::Extent2D mFramebufferSize = {};
    uint32_t mImageIndex = 0u;

    VulkanAllocator mAllocator;
    VulkanContext mContext;

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

    struct Implementation;
    std::unique_ptr<Implementation> impl;

    friend Implementation;
};

} // namesapce sq
