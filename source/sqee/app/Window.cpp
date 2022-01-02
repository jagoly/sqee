#include <sqee/app/Window.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/core/Algorithms.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/vk/VulkanContext.hpp>

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
    // annoying warning I always get, ignore it so that it doesn't trigger the breakpoint
    if (StringView("/usr/lib32/libvulkan_intel.so: wrong ELF class: ELFCLASS32") == pCallbackData->pMessage)
        return VK_FALSE;

    std::time_t now = std::time(nullptr);

    log_raw_multiline("{:%H:%M:%S} Vulkan: Severity = {} | Type = {}\n{}",
                      fmt::localtime(now),
                      vk::to_string(vk::DebugUtilsMessageSeverityFlagBitsEXT(messageSeverity)),
                      vk::to_string(vk::DebugUtilsMessageTypeFlagsEXT(messageType)),
                      pCallbackData->pMessage);

    return VK_FALSE;
}

//============================================================================//

constexpr Keyboard_Key impl_glfw_key_to_sqee(int key)
{
    switch (key) {

    //case GLFW_KEY_UNKNOWN:        return Keyboard_Key::Unknown;
    case GLFW_KEY_SPACE:          return Keyboard_Key::Space;
    case GLFW_KEY_APOSTROPHE:     return Keyboard_Key::Apostrophe;
    case GLFW_KEY_COMMA:          return Keyboard_Key::Comma;
    case GLFW_KEY_MINUS:          return Keyboard_Key::Dash;
    case GLFW_KEY_PERIOD:         return Keyboard_Key::Period;
    case GLFW_KEY_SLASH:          return Keyboard_Key::Slash;
    case GLFW_KEY_0:              return Keyboard_Key::Num_0;
    case GLFW_KEY_1:              return Keyboard_Key::Num_1;
    case GLFW_KEY_2:              return Keyboard_Key::Num_2;
    case GLFW_KEY_3:              return Keyboard_Key::Num_3;
    case GLFW_KEY_4:              return Keyboard_Key::Num_4;
    case GLFW_KEY_5:              return Keyboard_Key::Num_5;
    case GLFW_KEY_6:              return Keyboard_Key::Num_6;
    case GLFW_KEY_7:              return Keyboard_Key::Num_7;
    case GLFW_KEY_8:              return Keyboard_Key::Num_8;
    case GLFW_KEY_9:              return Keyboard_Key::Num_9;
    case GLFW_KEY_SEMICOLON:      return Keyboard_Key::SemiColon;
    case GLFW_KEY_EQUAL:          return Keyboard_Key::Equal;
    case GLFW_KEY_A:              return Keyboard_Key::A;
    case GLFW_KEY_B:              return Keyboard_Key::B;
    case GLFW_KEY_C:              return Keyboard_Key::C;
    case GLFW_KEY_D:              return Keyboard_Key::D;
    case GLFW_KEY_E:              return Keyboard_Key::E;
    case GLFW_KEY_F:              return Keyboard_Key::F;
    case GLFW_KEY_G:              return Keyboard_Key::G;
    case GLFW_KEY_H:              return Keyboard_Key::H;
    case GLFW_KEY_I:              return Keyboard_Key::I;
    case GLFW_KEY_J:              return Keyboard_Key::J;
    case GLFW_KEY_K:              return Keyboard_Key::K;
    case GLFW_KEY_L:              return Keyboard_Key::L;
    case GLFW_KEY_M:              return Keyboard_Key::M;
    case GLFW_KEY_N:              return Keyboard_Key::N;
    case GLFW_KEY_O:              return Keyboard_Key::O;
    case GLFW_KEY_P:              return Keyboard_Key::P;
    case GLFW_KEY_Q:              return Keyboard_Key::Q;
    case GLFW_KEY_R:              return Keyboard_Key::R;
    case GLFW_KEY_S:              return Keyboard_Key::S;
    case GLFW_KEY_T:              return Keyboard_Key::T;
    case GLFW_KEY_U:              return Keyboard_Key::U;
    case GLFW_KEY_V:              return Keyboard_Key::V;
    case GLFW_KEY_W:              return Keyboard_Key::W;
    case GLFW_KEY_X:              return Keyboard_Key::X;
    case GLFW_KEY_Y:              return Keyboard_Key::Y;
    case GLFW_KEY_Z:              return Keyboard_Key::Z;
    case GLFW_KEY_LEFT_BRACKET:   return Keyboard_Key::LeftBracket;
    case GLFW_KEY_BACKSLASH:      return Keyboard_Key::BackSlash;
    case GLFW_KEY_RIGHT_BRACKET:  return Keyboard_Key::RightBracket;
    case GLFW_KEY_GRAVE_ACCENT:   return Keyboard_Key::Grave;
    //case GLFW_KEY_WORLD_1:        return Keyboard_Key::Unknown;
    //case GLFW_KEY_WORLD_2:        return Keyboard_Key::Unknown;
    case GLFW_KEY_ESCAPE:         return Keyboard_Key::Escape;
    case GLFW_KEY_ENTER:          return Keyboard_Key::Return;
    case GLFW_KEY_TAB:            return Keyboard_Key::Tab;
    case GLFW_KEY_BACKSPACE:      return Keyboard_Key::BackSpace;
    case GLFW_KEY_INSERT:         return Keyboard_Key::Insert;
    case GLFW_KEY_DELETE:         return Keyboard_Key::Delete;
    case GLFW_KEY_RIGHT:          return Keyboard_Key::Arrow_Right;
    case GLFW_KEY_LEFT:           return Keyboard_Key::Arrow_Left;
    case GLFW_KEY_DOWN:           return Keyboard_Key::Arrow_Down;
    case GLFW_KEY_UP:             return Keyboard_Key::Arrow_Up;
    case GLFW_KEY_PAGE_UP:        return Keyboard_Key::PageUp;
    case GLFW_KEY_PAGE_DOWN:      return Keyboard_Key::PageDown;
    case GLFW_KEY_HOME:           return Keyboard_Key::Home;
    case GLFW_KEY_END:            return Keyboard_Key::End;
    //case GLFW_KEY_CAPS_LOCK:      return Keyboard_Key::Unknown;
    //case GLFW_KEY_SCROLL_LOCK:    return Keyboard_Key::Unknown;
    //case GLFW_KEY_NUM_LOCK:       return Keyboard_Key::Unknown;
    //case GLFW_KEY_PRINT_SCREEN:   return Keyboard_Key::Unknown;
    //case GLFW_KEY_PAUSE:          return Keyboard_Key::Unknown;
    case GLFW_KEY_F1:             return Keyboard_Key::F1;
    case GLFW_KEY_F2:             return Keyboard_Key::F2;
    case GLFW_KEY_F3:             return Keyboard_Key::F3;
    case GLFW_KEY_F4:             return Keyboard_Key::F4;
    case GLFW_KEY_F5:             return Keyboard_Key::F5;
    case GLFW_KEY_F6:             return Keyboard_Key::F6;
    case GLFW_KEY_F7:             return Keyboard_Key::F7;
    case GLFW_KEY_F8:             return Keyboard_Key::F8;
    case GLFW_KEY_F9:             return Keyboard_Key::F9;
    case GLFW_KEY_F10:            return Keyboard_Key::F10;
    case GLFW_KEY_F11:            return Keyboard_Key::F11;
    case GLFW_KEY_F12:            return Keyboard_Key::F12;
    //case GLFW_KEY_F13:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F14:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F15:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F16:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F17:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F18:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F19:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F20:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F21:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F22:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F23:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F24:            return Keyboard_Key::Unknown;
    //case GLFW_KEY_F25:            return Keyboard_Key::Unknown;
    case GLFW_KEY_KP_0:           return Keyboard_Key::Pad_0;
    case GLFW_KEY_KP_1:           return Keyboard_Key::Pad_1;
    case GLFW_KEY_KP_2:           return Keyboard_Key::Pad_2;
    case GLFW_KEY_KP_3:           return Keyboard_Key::Pad_3;
    case GLFW_KEY_KP_4:           return Keyboard_Key::Pad_4;
    case GLFW_KEY_KP_5:           return Keyboard_Key::Pad_5;
    case GLFW_KEY_KP_6:           return Keyboard_Key::Pad_6;
    case GLFW_KEY_KP_7:           return Keyboard_Key::Pad_7;
    case GLFW_KEY_KP_8:           return Keyboard_Key::Pad_8;
    case GLFW_KEY_KP_9:           return Keyboard_Key::Pad_9;
    case GLFW_KEY_KP_DECIMAL:     return Keyboard_Key::Pad_Decimal;
    case GLFW_KEY_KP_DIVIDE:      return Keyboard_Key::Pad_Divide;
    case GLFW_KEY_KP_MULTIPLY:    return Keyboard_Key::Pad_Multiply;
    case GLFW_KEY_KP_SUBTRACT:    return Keyboard_Key::Pad_Minus;
    case GLFW_KEY_KP_ADD:         return Keyboard_Key::Pad_Plus;
    case GLFW_KEY_KP_ENTER:       return Keyboard_Key::Pad_Return;
    //case GLFW_KEY_KP_EQUAL:       return Keyboard_Key::Unknown;
    case GLFW_KEY_LEFT_SHIFT:     return Keyboard_Key::Shift_L;
    case GLFW_KEY_LEFT_CONTROL:   return Keyboard_Key::Control_L;
    case GLFW_KEY_LEFT_ALT:       return Keyboard_Key::Alt_L;
    case GLFW_KEY_LEFT_SUPER:     return Keyboard_Key::Super_L;
    case GLFW_KEY_RIGHT_SHIFT:    return Keyboard_Key::Shift_R;
    case GLFW_KEY_RIGHT_CONTROL:  return Keyboard_Key::Control_R;
    case GLFW_KEY_RIGHT_ALT:      return Keyboard_Key::Alt_R;
    case GLFW_KEY_RIGHT_SUPER:    return Keyboard_Key::Super_R;
    case GLFW_KEY_MENU:           return Keyboard_Key::Menu;

    default: return Keyboard_Key::Unknown;

    } // switch(key)
}

//============================================================================//

constexpr Mouse_Button impl_glfw_mouse_button_to_sqee(int button)
{
    switch (button) {

    case GLFW_MOUSE_BUTTON_1: return Mouse_Button::Left;
    case GLFW_MOUSE_BUTTON_2: return Mouse_Button::Right;
    case GLFW_MOUSE_BUTTON_3: return Mouse_Button::Middle;
    case GLFW_MOUSE_BUTTON_4: return Mouse_Button::ExtraA;
    case GLFW_MOUSE_BUTTON_5: return Mouse_Button::ExtraB;

    default: return Mouse_Button::Unknown;

    } // switch(button)
}

//============================================================================//

struct Window::Implementation
{
    static void add_event(GLFWwindow* glfwWindow, Event event)
    {
        static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))->mEvents.push_back(event);
    }

    static void cb_window_close(GLFWwindow* window)
    {
        Event event;
        event.type = Event::Type::Window_Close;
        add_event(window, event);
    }

    static void cb_window_focus(GLFWwindow* window, int focused)
    {
        Event event;
        event.type = Event::Type(int(Event::Type::Window_Unfocus) + focused);
        add_event(window, event);
    }

    static void cb_key(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
    {
        Event event;
        event.type = Event::Type(int(Event::Type::Keyboard_Release) + action);
        event.data.keyboard.key = impl_glfw_key_to_sqee(key);
        event.data.keyboard.shift = mods & GLFW_MOD_SHIFT;
        event.data.keyboard.ctrl = mods & GLFW_MOD_CONTROL;
        event.data.keyboard.alt = mods & GLFW_MOD_ALT;
        event.data.keyboard.super = mods & GLFW_MOD_SUPER;
        add_event(window, event);
    }

    static void cb_mouse_button(GLFWwindow* window, int button, int action, int mods)
    {
        Event event;
        event.type = Event::Type(int(Event::Type::Mouse_Release) + action);
        event.data.mouse.button = impl_glfw_mouse_button_to_sqee(button);
        event.data.mouse.shift = mods & GLFW_MOD_SHIFT;
        event.data.mouse.ctrl = mods & GLFW_MOD_CONTROL;
        event.data.mouse.alt = mods & GLFW_MOD_ALT;
        event.data.mouse.super = mods & GLFW_MOD_SUPER;
        add_event(window, event);
    }

    static void cb_scroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        Event event;
        event.type = Event::Type::Mouse_Scroll;
        event.data.scroll.delta.x = float(xoffset);
        event.data.scroll.delta.y = float(yoffset);
        add_event(window, event);
    }

    static void cb_char(GLFWwindow* window, uint codepoint)
    {
        Event event;
        event.type = Event::Type::Text_Entry;
        event.data.text.unicode = codepoint;
        add_event(window, event);
    }
};

//============================================================================//

Window::Window(const char* title, Vec2U size, const char* appName, Vec3U version)
{
    // create glfw window
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        mGlfwWindow = glfwCreateWindow(int(size.x), int(size.y), title, nullptr, nullptr);

        glfwSetWindowUserPointer(mGlfwWindow, this);

        glfwSetWindowCloseCallback(mGlfwWindow, Implementation::cb_window_close);
        glfwSetWindowFocusCallback(mGlfwWindow, Implementation::cb_window_focus);
        glfwSetKeyCallback(mGlfwWindow, Implementation::cb_key);
        glfwSetMouseButtonCallback(mGlfwWindow, Implementation::cb_mouse_button);
        glfwSetScrollCallback(mGlfwWindow, Implementation::cb_scroll);
        glfwSetCharCallback(mGlfwWindow, Implementation::cb_char);

        // load functions required to create an instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(glfwGetInstanceProcAddress);
    }

    // setup instance and debug callback
    {
        const auto appInfo = vk::ApplicationInfo {
            appName, VK_MAKE_VERSION(version.x, version.y, version.z),
            "SQEE", VK_MAKE_VERSION(1u, 2u, 3u),
            VK_API_VERSION_1_2
        };

        const std::vector<const char*> layers = []()
        {
            const char* layerName = "VK_LAYER_KHRONOS_validation";

            for (const auto& layer : vk::enumerateInstanceLayerProperties())
                if (StringView(layer.layerName) == layerName)
                    return std::vector { layerName };

            log_warning("vulkan validation layer could not be loaded");
            return std::vector<const char*>();
        }();

        const std::vector<const char*> extensions = []()
        {
            uint32_t count = 0u;
            const char** names = glfwGetRequiredInstanceExtensions(&count);

            std::vector<const char*> result;
            result.reserve(count + 1u);

            for (uint32_t i = 0u; i < count; ++i)
                result.push_back(names[i]);

            result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            return result;
        }();

        const auto severityFlags = //vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                   //vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        const auto typeFlags = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        const auto validationEnables = std::array {
            //vk::ValidationFeatureEnableEXT::eBestPractices,
            vk::ValidationFeatureEnableEXT::eSynchronizationValidation
        };

        mInstance = vk::createInstance (
            vk::StructureChain {
                vk::InstanceCreateInfo { {}, &appInfo, layers, extensions },
                vk::DebugUtilsMessengerCreateInfoEXT { {}, severityFlags, typeFlags, impl_vulkan_debug_callback },
                vk::ValidationFeaturesEXT { validationEnables, {} }
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
            log_error("No GPUs found with Vulkan support");

        for (const auto& physDev : physicalDevices)
        {
            log_debug_multiline("Vulkan Physical Device Info\nName:    {} \nQueues:  {}\nPresent: {}",
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
            log_error("GPU(s) found, but none are useable");

        log_info("Using Vulkan Device '{}'", mPhysicalDevice.getProperties().deviceName);
    }

    // setup logical device and queue
    {
        auto priorities = std::array { 1.f };
        auto queues = vk::DeviceQueueCreateInfo { {}, 0u, priorities };
        auto extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

        auto features = vk::PhysicalDeviceFeatures();
        features.fillModeNonSolid = true;
        features.samplerAnisotropy = true;
        features.wideLines = true;
        features.geometryShader = true;
        features.depthClamp = true;

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
        // todo: allow these to be configured
        auto poolSizes = std::array {
            vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, 512u },
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 512u }
        };

        mDesciptorPool = mDevice.createDescriptorPool (
            vk::DescriptorPoolCreateInfo { vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 512u, poolSizes }
        );
    }

    // create render pass
    {
        auto attachments = vk::AttachmentDescription {
            {}, vk::Format::eB8G8R8A8Srgb, vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
        };

        auto colorAttachments = vk::AttachmentReference {
            0u, vk::ImageLayout::eColorAttachmentOptimal
        };

        auto subpasses = vk::SubpassDescription {
            {}, vk::PipelineBindPoint::eGraphics, {}, colorAttachments, {}, nullptr, {}
        };

//        auto dependencies = std::array {
//            vk::SubpassDependency {
//                VK_SUBPASS_EXTERNAL, 0u,
//                vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eFragmentShader,
//                vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eShaderRead,
//                vk::DependencyFlagBits::eByRegion
//            }
//        };

        mRenderPass = mDevice.createRenderPass (
            vk::RenderPassCreateInfo { {}, attachments, subpasses, {}/*dependencies*/ }
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
        VulkanContext::construct(mAllocator);
        VulkanContext& context = VulkanContext::get_mutable();

        context.device = mDevice;
        context.queue = mQueue;
        context.commandPool = mCommandPool;
        context.descriptorPool = mDesciptorPool;

        // query limits
        {
            const auto limits = mPhysicalDevice.getProperties().limits;

            context.limits.maxAnisotropy = limits.maxSamplerAnisotropy;
            context.limits.timestampPeriod = limits.timestampPeriod;
        }
    }
}

//============================================================================//

Window::~Window()
{
    mDevice.waitIdle();

    VulkanContext::destruct();

    mAllocator.destroy();

    destroy_swapchain_and_friends();

    mDevice.destroy(mRenderPass);

    mDevice.destroy(mImageAvailableSemaphore.front);
    mDevice.destroy(mImageAvailableSemaphore.back);
    mDevice.destroy(mRenderFinishedSemaphore.front);
    mDevice.destroy(mRenderFinishedSemaphore.back);
    mDevice.destroy(mRenderFinishedFence.front);
    mDevice.destroy(mRenderFinishedFence.back);

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

void Window::set_size_limits(std::optional<Vec2U> minimum, std::optional<Vec2U> maximum)
{
    // intended to be called before creating the swapchain

    const Vec2I signedMin = minimum.has_value() ? Vec2I(minimum.value()) : Vec2I(GLFW_DONT_CARE);
    const Vec2I signedMax = maximum.has_value() ? Vec2I(maximum.value()) : Vec2I(GLFW_DONT_CARE);

    glfwSetWindowSizeLimits(mGlfwWindow, signedMin.x, signedMin.y, signedMax.x, signedMax.y);
}

//============================================================================//

void Window::create_swapchain_and_friends()
{
    const auto capabilities = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);

    mFramebufferSize = capabilities.currentExtent;

    // create swapchain
    {
        const auto presentMode = mVsyncEnabled ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate;

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
    }

    // create image views and framebuffers
    {
        mSwapchainImageViews.reserve(mSwapchainImages.size());
        mSwapchainFramebuffers.reserve(mSwapchainImageViews.size());

        for (auto& image : mSwapchainImages)
        {
            mSwapchainImageViews.emplace_back() = mDevice.createImageView (
                vk::ImageViewCreateInfo {
                    {}, image, vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, {},
                    vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u)
                }
            );

            mSwapchainFramebuffers.emplace_back() = mDevice.createFramebuffer (
                vk::FramebufferCreateInfo {
                    {}, mRenderPass, mSwapchainImageViews.back(),
                    mFramebufferSize.width, mFramebufferSize.height, 1u
                }
            );
        }
    }

    mPendingResize = false;
}

//============================================================================//

void Window::destroy_swapchain_and_friends()
{
    mDevice.waitIdle();

    for (auto& framebuffer : mSwapchainFramebuffers)
        mDevice.destroy(framebuffer);

    for (auto& imageView : mSwapchainImageViews)
        mDevice.destroy(imageView);

    mDevice.destroy(mSwapchain);

    mSwapchainFramebuffers.clear();
    mSwapchainImageViews.clear();
}

//============================================================================//

const std::vector<Event>& Window::fetch_events()
{
    glfwPollEvents();

    // some drivers don't request resize after acquire or present
    if (mPendingResize == false)
    {
        int newWidth, newHeight;
        glfwGetFramebufferSize(mGlfwWindow, &newWidth, &newHeight);

        if (mFramebufferSize != vk::Extent2D(uint(newWidth), uint(newHeight)))
        {
            mEvents.push_back({Event::Type::Window_Resize, {}});
            mPendingResize = true;
        }
    }

    std::swap(mEvents, mEventsOld);
    mEvents.clear();

    return mEventsOld;
}

//============================================================================//

void Window::begin_frame()
{
    auto waitResult = mDevice.waitForFences(mRenderFinishedFence.front, true, UINT64_MAX);
    SQASSERT(waitResult == vk::Result::eSuccess, "");
}

//============================================================================//

std::tuple<vk::CommandBuffer, vk::Framebuffer> Window::acquire_image()
{
    if (mPendingResize == true)
        return { {}, {} }; // EARLY RETURN

    const uint32_t oldImageIndex = mImageIndex;

    // use non-throwing function so that resizing doesn't trigger a break
    {
        auto result = mDevice.acquireNextImageKHR (
            mSwapchain, UINT64_MAX, mImageAvailableSemaphore.front, nullptr, &mImageIndex
        );

        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            mEvents.push_back({Event::Type::Window_Resize, {}});
            mPendingResize = true;

            return { {}, {} }; // EARLY RETURN
        }
        else SQASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "");
    }

    if (oldImageIndex == mImageIndex)
        mDevice.waitIdle();

    mCommandBuffer.front.reset({});

    return { mCommandBuffer.front, mSwapchainFramebuffers[mImageIndex] };
}

//============================================================================//

void Window::submit_present_swap()
{
    SQASSERT(mPendingResize == false, "attempt to present during resize");

    mDevice.resetFences(mRenderFinishedFence.front);

    auto waitDstStageMask = vk::Flags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    mQueue.submit (
        vk::SubmitInfo {
            mImageAvailableSemaphore.front, waitDstStageMask,
            mCommandBuffer.front, mRenderFinishedSemaphore.front,
        },
        mRenderFinishedFence.front
    );

    // use non-throwing function so that resizing doesn't trigger a break
    {
        auto presentInfo = vk::PresentInfoKHR { mRenderFinishedSemaphore.front, mSwapchain, mImageIndex, {} };
        auto result = mQueue.presentKHR(&presentInfo);

        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            mEvents.push_back({Event::Type::Window_Resize, {}});
            mPendingResize = true;
        }
        else SQASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "");
    }

    mCommandBuffer.swap();
    mImageAvailableSemaphore.swap();
    mRenderFinishedSemaphore.swap();
    mRenderFinishedFence.swap();
}

//============================================================================//

void Window::set_title(String title)
{
    mTitle = std::move(title);
    glfwSetWindowTitle(mGlfwWindow, mTitle.c_str());
}

// todo: GLFW supports proper raw mouse movement, unlike SFML
void Window::set_cursor_hidden(bool hidden)
{
    if (mCursorHidden == hidden) return;

    mCursorHidden = hidden;
    glfwSetInputMode(mGlfwWindow, GLFW_CURSOR, hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

// todo: Vulkan supports freesync/gsync refresh, unline OpenGL
void Window::set_vsync_enabled(bool enabled)
{
    mVsyncEnabled = enabled;
}

//============================================================================//

bool Window::has_focus() const
{
    return bool(glfwGetWindowAttrib(mGlfwWindow, GLFW_FOCUSED));
}
