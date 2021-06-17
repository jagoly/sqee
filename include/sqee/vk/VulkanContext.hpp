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
        float timestampPeriod;
    }
    limits;

    //--------------------------------------------------------//

    /// Give an object a human readable name for debugging.
    template <class Object>
    void set_debug_object_name(Object object [[maybe_unused]], const char* name [[maybe_unused]]) const
    {
      #ifdef SQEE_DEBUG
        auto handle = reinterpret_cast<uint64_t>(static_cast<typename Object::CType>(object));
        device.setDebugUtilsObjectNameEXT({object.objectType, handle, name});
      #endif
    }

private: //===================================================//

    VulkanContext(VulkanAllocator& allocator);

    static void construct(VulkanAllocator& allocator);

    static void destruct();

    static VulkanContext& get_mutable();

    friend Window;
};

//============================================================================//

} // namespace sq
