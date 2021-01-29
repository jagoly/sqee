#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class VulkanAllocator;
class VulkWindow;

//============================================================================//

class SQEE_API VulkanContext final : NonCopyable
{
public: //====================================================//

    /// Access the static VulkanContext instance.
    static const VulkanContext& get();

    //--------------------------------------------------------//

    VulkanAllocator& allocator;

    vk::Device device;
    vk::Queue queue;
    vk::CommandPool commandPool;
    vk::DescriptorPool descriptorPool;

    //--------------------------------------------------------//

    /// Window information that gets updated each frame.
    struct Window
    {
        Vec2U size;
        vk::Framebuffer framebuffer;
    }
    window;

    //--------------------------------------------------------//

    /// Relevant hardware limits from device.
    struct
    {
        float maxAnisotropy;
    }
    limits;

private: //===================================================//

    VulkanContext(VulkanAllocator& allocator);

    static void construct(VulkanAllocator& allocator);

    static void destruct();

    static VulkanContext& get_mutable();

    friend VulkWindow;
};

//============================================================================//

} // namespace sq
