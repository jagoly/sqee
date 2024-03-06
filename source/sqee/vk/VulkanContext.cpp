#include <sqee/vk/VulkanContext.hpp>

#include <sqee/debug/Assert.hpp>

using namespace sq;

//==============================================================================

static VulkanContext* gContextPtr = nullptr;

const VulkanContext& VulkanContext::get() { return *gContextPtr; }

//==============================================================================

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

//==============================================================================

vk::DescriptorSet VulkanContext::allocate_descriptor_set(vk::DescriptorPool pool, vk::DescriptorSetLayout layout) const
{
    const auto info = vk::DescriptorSetAllocateInfo { pool, layout };

    vk::DescriptorSet descriptorSet;

    const auto result = device.allocateDescriptorSets(&info, &descriptorSet);
    if (result != vk::Result::eSuccess)
        vk::detail::throwResultException(result, "VulkanContext::allocate_descriptor_set");

    return descriptorSet;
}
