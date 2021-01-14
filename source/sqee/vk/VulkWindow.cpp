#include <sqee/vk/VulkWindow.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/core/Algorithms.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <fmt/chrono.h>

using namespace sq;

//============================================================================//

VKAPI_ATTR VkBool32 VKAPI_CALL impl_vulkan_debug_callback (
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* /*pUserData*/ )
{
    std::time_t now = std::time(nullptr);
    sq::log_raw_multiline("{:%H:%M:%S} Vulkan: Severity = {} | Type = {}\n{}",
                          fmt::localtime(now),
                          vk::to_string(vk::DebugUtilsMessageSeverityFlagBitsEXT(messageSeverity)),
                          vk::to_string(vk::DebugUtilsMessageTypeFlagsEXT(messageType)),
                          pCallbackData->pMessage);

    return VK_FALSE;
}

//============================================================================//

struct VulkWindow::Implementation
{
    static inline VulkWindow* get(GLFWwindow* glfwWindow)
    {
        return static_cast<VulkWindow*>(glfwGetWindowUserPointer(glfwWindow));
    }

    static void cb_window_close(GLFWwindow* window)
    {
        Event event { Event::Type::Window_Close, {} };
        get(window)->impl->events.push_back(event);
    }

    std::vector<Event> events, eventsOld;
};

//============================================================================//

sq::VulkWindow::VulkWindow(const char* title, sq::Vec2U size, const char* appName, int vMajor, int vMinor, int vPatch)
{
    impl = std::make_unique<Implementation>();

    // create glfw window
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mGlfwWindow = glfwCreateWindow(int(size.x), int(size.y), title, nullptr, nullptr);

        auto window = static_cast<GLFWwindow*>(mGlfwWindow);

        glfwSetWindowUserPointer(window, this);

        glfwSetWindowCloseCallback(window, Implementation::cb_window_close);
    }

    // initial dispatcher setup
    {
        vk::DynamicLoader dl;
        auto gipa = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(gipa);
        //VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    }

    // setup instance, surface, and debug callback
    {
        vk::ApplicationInfo appInfo {
            appName, VK_MAKE_VERSION(vMajor,vMinor,vPatch), "SQEE", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_2
        };

        const std::array layers = { "VK_LAYER_KHRONOS_validation" };

        uint32_t extCount = 0u;
        const char** extNames = glfwGetRequiredInstanceExtensions(&extCount);

        std::vector<const char*> extensions;
        extensions.reserve(extCount + 1u);

        for (uint32_t i = 0u; i < extCount; ++i)
            extensions.push_back(extNames[i]);

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        const auto severityFlags = //vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        const auto typeFlags = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        mInstance = vk::createInstance (
            vk::StructureChain {
                vk::InstanceCreateInfo { {}, &appInfo, layers, extensions },
                vk::DebugUtilsMessengerCreateInfoEXT { {}, severityFlags, typeFlags, impl_vulkan_debug_callback }
            }.get()
        );

        VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance);

        mDebugMessenger = mInstance.createDebugUtilsMessengerEXT (
            vk::DebugUtilsMessengerCreateInfoEXT { {}, severityFlags, typeFlags, impl_vulkan_debug_callback }
        );
    }

    // create window surface
    {
        auto window = static_cast<GLFWwindow*>(mGlfwWindow);
        auto surfacePtr = reinterpret_cast<VkSurfaceKHR*>(&mSurface);

        if (glfwCreateWindowSurface(mInstance, window, nullptr, surfacePtr) != VK_SUCCESS)
            sq::log_error("failed to create window surface");
    }

    // setup physical device
    {
        const auto physicalDevices = mInstance.enumeratePhysicalDevices();
        if (physicalDevices.empty())
            sq::log_error("No GPUs found with Vulkan support");

        for (const auto& physDev : physicalDevices)
        {
            sq::log_debug_multiline("Vulkan Physical Device Info\nName:    {} \nQueues:  {}\nPresent: {}",
                                    physDev.getProperties().deviceName,
                                    vk::to_string(physDev.getQueueFamilyProperties().front().queueFlags),
                                    bool(physDev.getSurfaceSupportKHR(0u, mSurface)));

            // check that device is supported
            {
                if (!(physDev.getQueueFamilyProperties().front().queueFlags & vk::QueueFlagBits::eGraphics))
                    continue;

                if (!physDev.getSurfaceSupportKHR(0u, mSurface))
                    continue;

                const auto formats = physDev.getSurfaceFormatsKHR(mSurface);
                if (algo::none_of(formats, [](auto& f) { return f.format == vk::Format::eB8G8R8A8Srgb; }))
                    continue;
            }

            const auto preferredType = vk::PhysicalDeviceType::eDiscreteGpu;
            //const auto preferredType = vk::PhysicalDeviceType::eIntegratedGpu;

            if (mPhysicalDevice)
            {
                if (mPhysicalDevice.getProperties().deviceType != preferredType)
                    if (physDev.getProperties().deviceType == preferredType)
                        mPhysicalDevice = physDev;
            }
            else mPhysicalDevice = physDev;
        }

        if (!mPhysicalDevice)
            sq::log_error("GPU(s) found, but none are useable");

        sq::log_info("Using Vulkan Device '{}'", mPhysicalDevice.getProperties().deviceName);
    }

    // setup logical device and queue
    {
        auto priorities = std::array { 1.f };
        auto queues = vk::DeviceQueueCreateInfo { {}, 0u, priorities };
        auto extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

        mDevice = mPhysicalDevice.createDevice (
            vk::DeviceCreateInfo { {}, queues, {}, extensions, {} }
        );

        mQueue = mDevice.getQueue(0u, 0u);
    }

    // create semaphores and fences
    {
        mImageAvailableSemaphore = mDevice.createSemaphore({});
        mImageAvailableSemaphoreOther = mDevice.createSemaphore({});

        mRenderFinishedSemaphore = mDevice.createSemaphore({});
        mRenderFinishedSemaphoreOther = mDevice.createSemaphore({});

        mRenderFinishedFence = mDevice.createFence({vk::FenceCreateFlagBits::eSignaled});
        mRenderFinishedFenceOther = mDevice.createFence({vk::FenceCreateFlagBits::eSignaled});
    }

    // create command pool
    {
        mCommandPool = mDevice.createCommandPool (
            vk::CommandPoolCreateInfo { {}, 0u }
        );
    }

    create_swapchain_and_friends();

    allocate_command_buffers();
}

//============================================================================//

VulkWindow::~VulkWindow()
{
    mDevice.waitIdle();

    destroy_swapchain_and_friends();

    mDevice.destroy(mImageAvailableSemaphore);
    mDevice.destroy(mImageAvailableSemaphoreOther);
    mDevice.destroy(mRenderFinishedSemaphore);
    mDevice.destroy(mRenderFinishedSemaphoreOther);
    mDevice.destroy(mRenderFinishedFence);
    mDevice.destroy(mRenderFinishedFenceOther);

    mDevice.destroy(mCommandPool);
    mDevice.destroy();

    mInstance.destroy(mSurface);
    mInstance.destroy(mDebugMessenger);
    mInstance.destroy();

    glfwDestroyWindow(static_cast<GLFWwindow*>(mGlfwWindow));
    glfwTerminate();
}

//============================================================================//

void VulkWindow::create_swapchain_and_friends()
{
    // get framebuffer size
    {
        auto window = static_cast<GLFWwindow*>(mGlfwWindow);
        int intWidth, intHeight;
        glfwGetFramebufferSize(window, &intWidth, &intHeight);
        mFramebufferSize = { uint(intWidth), uint(intHeight) };
    }

    // create swapchain
    {
        const auto capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

        mSwapchain = mDevice.createSwapchainKHR (
            vk::SwapchainCreateInfoKHR {
                {}, mSurface, capabilities.minImageCount , vk::Format::eB8G8R8A8Srgb,
                vk::ColorSpaceKHR::eSrgbNonlinear, { mFramebufferSize.x, mFramebufferSize.y }, 1u,
                vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
                vk::SurfaceTransformFlagBitsKHR::eIdentity, vk::CompositeAlphaFlagBitsKHR::eOpaque,
                vk::PresentModeKHR::eImmediate, true, nullptr
            }
        );

        mSwapchainImages = mDevice.getSwapchainImagesKHR(mSwapchain);

        mSwapchainImageViews.clear();
        mSwapchainImageViews.reserve(mSwapchainImages.size());

        for (auto& image : mSwapchainImages)
        {
            mSwapchainImageViews.emplace_back() = mDevice.createImageView (
                vk::ImageViewCreateInfo {
                    {}, image, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, {},
                    vk::ImageSubresourceRange { vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u }
                }
            );
        }

        sq::log_debug("Swapchain Info: {}, {}, {}", mFramebufferSize.x, mFramebufferSize.y, mSwapchainImages.size());
    }

    // create render pass
    {
        auto attachments = vk::AttachmentDescription {
            {}, vk::Format::eB8G8R8A8Srgb, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
        };

        auto colorAttachments = vk::AttachmentReference {
            0u, vk::ImageLayout::eColorAttachmentOptimal
        };

        auto subpasses = vk::SubpassDescription {
            {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachments
        };

        auto dependencies = vk::SubpassDependency {
            VK_SUBPASS_EXTERNAL, 0u,
            vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
            {}, vk::AccessFlagBits::eColorAttachmentWrite, {}
        };

        mRenderPass = mDevice.createRenderPass (
            vk::RenderPassCreateInfo {
                {}, attachments, subpasses, dependencies
            }
        );
    }

    // create framebuffers
    {
        mSwapchainFramebuffers.clear();
        mSwapchainFramebuffers.reserve(get_swapchain_image_count());

        for (auto& imageView : mSwapchainImageViews)
        {
            mSwapchainFramebuffers.emplace_back() = mDevice.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mRenderPass, imageView, mFramebufferSize.x, mFramebufferSize.y, 1u
                }
            );
        }
    }

    mNeedWaitFenceOther = true;
}

//============================================================================//

void VulkWindow::destroy_swapchain_and_friends()
{
    for (auto& framebuffer : mSwapchainFramebuffers)
        mDevice.destroy(framebuffer);

    free_command_buffers();

    mDevice.destroy(mRenderPass);

    for (auto& imageView : mSwapchainImageViews)
        mDevice.destroy(imageView);

    mDevice.destroy(mSwapchain);

    mSwapchainFramebuffers.clear();
    mSwapchainImageViews.clear();
}

//============================================================================//

void VulkWindow::allocate_command_buffers()
{
    mCommandBuffers = mDevice.allocateCommandBuffers (
        vk::CommandBufferAllocateInfo {
            mCommandPool, vk::CommandBufferLevel::ePrimary, get_swapchain_image_count()
        }
    );
}

void VulkWindow::free_command_buffers()
{
    mDevice.free(mCommandPool, mCommandBuffers);
    mCommandBuffers.clear();
}

//============================================================================//

const std::vector<Event>& VulkWindow::fetch_events()
{
    glfwPollEvents();

    // we don't want to resize more than once at a time
    const auto predicate = [](Event& event) { return event.type == Event::Type::Window_Resize; };
    const auto end = std::prev(std::find_if(impl->events.rbegin(), impl->events.rend(), predicate).base());
    impl->events.erase(std::remove_if(impl->events.begin(), end, predicate), end);

    std::swap(impl->events, impl->eventsOld);
    impl->events.clear();

    return impl->eventsOld;
}

//============================================================================//

void VulkWindow::handle_window_resize()
{
    mDevice.waitIdle();

    destroy_swapchain_and_friends();
    create_swapchain_and_friends();

    free_command_buffers();
    allocate_command_buffers();
}

//============================================================================//

void VulkWindow::submit_and_present()
{
    auto waitResult = mDevice.waitForFences(mRenderFinishedFence, true, UINT64_MAX);
    SQASSERT(waitResult == vk::Result::eSuccess, "");

    uint32_t imageIndex;
    try
    {
        imageIndex = mDevice.acquireNextImageKHR(mSwapchain, UINT64_MAX, mImageAvailableSemaphore, nullptr).value;
    }
    catch (const vk::OutOfDateKHRError&)
    {
        impl->events.push_back({ Event::Type::Window_Resize, {} });
        return;
    }

    if (mNeedWaitFenceOther == true)
    {
        auto waitResultOther = mDevice.waitForFences(mRenderFinishedFenceOther, true, UINT64_MAX);
        SQASSERT(waitResultOther == vk::Result::eSuccess, "");
        mNeedWaitFenceOther = false;
    }

    mDevice.resetFences(mRenderFinishedFence);

    auto waitDstStageMask = vk::Flags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    mQueue.submit (
        vk::SubmitInfo {
            mImageAvailableSemaphore, waitDstStageMask, mCommandBuffers[imageIndex], mRenderFinishedSemaphore,
        },
        mRenderFinishedFence
    );

    try
    {
        void( mQueue.presentKHR({mRenderFinishedSemaphore, mSwapchain, imageIndex, {}}) );
    }
    catch (const vk::OutOfDateKHRError&)
    {
        impl->events.push_back({ Event::Type::Window_Resize, {} });
    }

    std::swap(mImageAvailableSemaphore, mImageAvailableSemaphoreOther);
    std::swap(mRenderFinishedSemaphore, mRenderFinishedSemaphoreOther);
    std::swap(mRenderFinishedFence, mRenderFinishedFenceOther);
}

//============================================================================//

const vk::Device& VulkWindow::get_device() const
{
    return mDevice;
}

const vk::RenderPass& VulkWindow::get_renderpass() const
{
    return mRenderPass;
}

const std::vector<vk::Framebuffer>& VulkWindow::get_framebuffers() const
{
    return mSwapchainFramebuffers;
}

const std::vector<vk::CommandBuffer>& VulkWindow::get_commandbuffers() const
{
    return mCommandBuffers;
}

uint VulkWindow::get_swapchain_image_count() const
{
    return uint(mSwapchainImages.size());
}

Vec2U VulkWindow::get_framebuffer_size() const
{
    return mFramebufferSize;
}

//============================================================================//

void VulkWindow::set_title(String title)
{
    auto window = static_cast<GLFWwindow*>(mGlfwWindow);
    glfwSetWindowTitle(window, title.c_str());
}
