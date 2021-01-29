#include <sqee/vk/VulkanContext.hpp>

#include <sqee/debug/Assert.hpp>

using namespace sq;

//============================================================================//

static VulkanContext* gContextPtr = nullptr;

const VulkanContext& VulkanContext::get() { return *gContextPtr; }

//============================================================================//

VulkanContext::VulkanContext(VulkanAllocator& _allocator) : allocator(_allocator) {}

void VulkanContext::construct(VulkanAllocator& allocator)
{
    SQASSERT(gContextPtr == nullptr, "");
    gContextPtr = new VulkanContext(allocator);
}

void VulkanContext::destruct()
{
    SQASSERT(gContextPtr != nullptr, "");
    delete gContextPtr;
    gContextPtr = nullptr;
}

VulkanContext& VulkanContext::get_mutable() { return *gContextPtr; }
