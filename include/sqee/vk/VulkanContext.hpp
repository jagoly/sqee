#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class VulkanAllocator;
class Window;

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

    friend Window;
};

//============================================================================//

} // namespace sq
