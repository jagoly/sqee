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

VulkWindow::VulkWindow(const char* title, Vec2U size, const char* appName, Vec3U version)
{
    impl = std::make_unique<Implementation>();

    // create glfw window
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        mGlfwWindow = glfwCreateWindow(int(size.x), int(size.y), title, nullptr, nullptr);

        glfwSetWindowUserPointer(mGlfwWindow, this);

        glfwSetWindowCloseCallback(mGlfwWindow, Implementation::cb_window_close);
    }

    // initial dispatcher setup
    {
        vk::DynamicLoader dl;
        auto gipa = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(gipa);
        //VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    }

    // setup instance and debug callback
    {
        const auto appInfo = vk::ApplicationInfo {
            appName, VK_MAKE_VERSION(version.x, version.y, version.z),
            "SQEE", VK_MAKE_VERSION(1u, 2u, 3u),
            VK_API_VERSION_1_2
        };

        const auto layers = std::array { "VK_LAYER_KHRONOS_validation" };

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
        auto surfacePtr = reinterpret_cast<VkSurfaceKHR*>(&mSurface);
        if (glfwCreateWindowSurface(mInstance, mGlfwWindow, nullptr, surfacePtr) != VK_SUCCESS)
            log_error("failed to create window surface");
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

            //const auto preferredType = vk::PhysicalDeviceType::eDiscreteGpu;
            const auto preferredType = vk::PhysicalDeviceType::eIntegratedGpu;

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

        auto features = vk::PhysicalDeviceFeatures();
        features.samplerAnisotropy = true;

        mDevice = mPhysicalDevice.createDevice (
            vk::DeviceCreateInfo { {}, queues, {}, extensions, &features }
        );

        mQueue = mDevice.getQueue(0u, 0u);
    }

    // create semaphores and fences
    {
        mImageAvailableSemaphore.front = mDevice.createSemaphore({});
        mImageAvailableSemaphore.back = mDevice.createSemaphore({});

        mRenderFinishedSemaphore.front = mDevice.createSemaphore({});
        mRenderFinishedSemaphore.back = mDevice.createSemaphore({});

        mRenderFinishedFence.front = mDevice.createFence({vk::FenceCreateFlagBits::eSignaled});
        mRenderFinishedFence.back = mDevice.createFence({vk::FenceCreateFlagBits::eSignaled});
    }

    // create command pool and buffers
    {
        mCommandPool = mDevice.createCommandPool (
            vk::CommandPoolCreateInfo { vk::CommandPoolCreateFlagBits::eResetCommandBuffer, 0u }
        );

        mCommandBuffer.front = mDevice.allocateCommandBuffers (
            vk::CommandBufferAllocateInfo { mCommandPool, vk::CommandBufferLevel::ePrimary, 1u }
        ).front();

        mCommandBuffer.back = mDevice.allocateCommandBuffers (
            vk::CommandBufferAllocateInfo { mCommandPool, vk::CommandBufferLevel::ePrimary, 1u }
        ).front();
    }

    // create descriptor pool
    {
        auto poolSizes = std::array {
            vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, 256u },
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 512u }
        };

        mDesciptorPool = mDevice.createDescriptorPool (
            vk::DescriptorPoolCreateInfo { {}, 256u + 512u, poolSizes }
        );
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

    // create allocator
    {
        const auto memoryProps = mPhysicalDevice.getMemoryProperties();

        const auto host = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        const auto device = vk::MemoryPropertyFlagBits::eDeviceLocal;

        uint32_t memoryTypeHost = 0u;
        for (; memoryTypeHost < memoryProps.memoryTypeCount; ++memoryTypeHost)
            if ((memoryProps.memoryTypes[memoryTypeHost].propertyFlags & host) == host)
                break;

        uint32_t memoryTypeDevice = 0u;
        for (; memoryTypeDevice < memoryProps.memoryTypeCount; ++memoryTypeDevice)
            if ((memoryProps.memoryTypes[memoryTypeDevice].propertyFlags & device) == device)
                break;

        mAllocator.initialise(mDevice, memoryTypeHost, memoryTypeDevice, 4096u);
    }

    // setup context
    {
        mContext.device = mDevice;
        mContext.queue = mQueue;
        mContext.commandPool = mCommandPool;
        mContext.descriptorPool = mDesciptorPool;
        mContext.renderPass = mRenderPass;

        // query limits
        {
            const auto limits = mPhysicalDevice.getProperties().limits;

            mContext.limits.maxAnisotropy = limits.maxSamplerAnisotropy;
        }

        mContext.allocator = &mAllocator;
    }

    create_swapchain_and_friends();
}

//============================================================================//

VulkWindow::~VulkWindow()
{
    mDevice.waitIdle();

    mAllocator.destroy();

    destroy_swapchain_and_friends();

    mDevice.destroy(mImageAvailableSemaphore.front);
    mDevice.destroy(mImageAvailableSemaphore.back);
    mDevice.destroy(mRenderFinishedSemaphore.front);
    mDevice.destroy(mRenderFinishedSemaphore.back);
    mDevice.destroy(mRenderFinishedFence.front);
    mDevice.destroy(mRenderFinishedFence.back);

    mDevice.destroy(mRenderPass);

    mDevice.destroy(mDesciptorPool);
    mDevice.destroy(mCommandPool);
    mDevice.destroy();

    mInstance.destroy(mSurface);
    mInstance.destroy(mDebugMessenger);
    mInstance.destroy();

    glfwDestroyWindow(mGlfwWindow);
    glfwTerminate();
}

//============================================================================//

void VulkWindow::create_swapchain_and_friends()
{
    const auto capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

    // get framebuffer size
    {
        mFramebufferSize = capabilities.currentExtent;
        mContext.windowSize = { mFramebufferSize.width, mFramebufferSize.height };
    }

    // create swapchain
    {
        //const auto presentMode = vk::PresentModeKHR::eImmediate;
        const auto presentMode = vk::PresentModeKHR::eFifo;

        mSwapchain = mDevice.createSwapchainKHR (
            vk::SwapchainCreateInfoKHR {
                {}, mSurface, capabilities.minImageCount, vk::Format::eB8G8R8A8Srgb,
                vk::ColorSpaceKHR::eSrgbNonlinear, mFramebufferSize, 1u,
                vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, {},
                vk::SurfaceTransformFlagBitsKHR::eIdentity, vk::CompositeAlphaFlagBitsKHR::eOpaque,
                presentMode, true, nullptr
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

        //sq::log_debug("Swapchain: mode = {} | images = {} | size = {}×{}", vk::to_string(presentMode),
        //              mSwapchainImages.size(), mFramebufferSize.width, mFramebufferSize.height);
    }

    // create framebuffers
    {
        mSwapchainFramebuffers.clear();
        mSwapchainFramebuffers.reserve(mSwapchainImageViews.size());

        for (auto& imageView : mSwapchainImageViews)
        {
            mSwapchainFramebuffers.emplace_back() = mDevice.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mRenderPass, imageView, mFramebufferSize.width, mFramebufferSize.height, 1u
                }
            );
        }
    }
}

//============================================================================//

void VulkWindow::destroy_swapchain_and_friends()
{
    for (auto& framebuffer : mSwapchainFramebuffers)
        mDevice.destroy(framebuffer);

    for (auto& imageView : mSwapchainImageViews)
        mDevice.destroy(imageView);

    mDevice.destroy(mSwapchain);

    mSwapchainFramebuffers.clear();
    mSwapchainImageViews.clear();
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

bool VulkWindow::begin_new_frame()
{
    auto waitResult = mDevice.waitForFences(mRenderFinishedFence.front, true, UINT64_MAX);
    SQASSERT(waitResult == vk::Result::eSuccess, "");

    const uint32_t oldImageIndex = mImageIndex;

    try
    {
        mImageIndex = mDevice.acquireNextImageKHR (
            mSwapchain, UINT64_MAX, mImageAvailableSemaphore.front, nullptr
        ).value;
    }
    catch (const vk::OutOfDateKHRError&)
    {
        impl->events.push_back({ Event::Type::Window_Resize, {} });
        return false; // EARLY RETURN
    }

    if (oldImageIndex == mImageIndex)
        mDevice.waitIdle();

    mContext.frame.commandBuffer = mCommandBuffer.front;
    mContext.frame.framebuffer = mSwapchainFramebuffers[mImageIndex];

    mCommandBuffer.front.reset({});

    return true;
}

//============================================================================//

void VulkWindow::submit_and_present()
{
    mDevice.resetFences(mRenderFinishedFence.front);

    auto waitDstStageMask = vk::Flags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    mQueue.submit (
        vk::SubmitInfo {
            mImageAvailableSemaphore.front, waitDstStageMask,
            mCommandBuffer.front, mRenderFinishedSemaphore.front,
        },
        mRenderFinishedFence.front
    );

    try
    {
        auto presentResult = mQueue.presentKHR (
            vk::PresentInfoKHR { mRenderFinishedSemaphore.front, mSwapchain, mImageIndex, {} }
        );
        SQASSERT(presentResult == vk::Result::eSuccess, "");
    }
    catch (const vk::OutOfDateKHRError&)
    {
        impl->events.push_back({ Event::Type::Window_Resize, {} });
    }

    mCommandBuffer.swap();
    mImageAvailableSemaphore.swap();
    mRenderFinishedSemaphore.swap();
    mRenderFinishedFence.swap();
}

//============================================================================//

void VulkWindow::set_title(String title)
{
    glfwSetWindowTitle(mGlfwWindow, title.c_str());
}
