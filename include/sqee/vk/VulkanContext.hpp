#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq { //###############################################################

class VulkanAllocator;
class Window;

template <class T>
using ArrayProxyRef = const vk::ArrayProxy<const T>&;

//==============================================================================

class SQEE_API VulkanContext
{
public: //======================================================

    SQEE_COPY_DELETE(VulkanContext)
    SQEE_MOVE_DELETE(VulkanContext)

    /// Access the static VulkanContext instance.
    static const VulkanContext& get();

    //----------------------------------------------------------

    VulkanAllocator& allocator;

    vk::Device device;
    vk::Queue queue;
    vk::CommandPool commandPool;
    vk::DescriptorPool descriptorPool;

    //----------------------------------------------------------

    /// Relevant hardware limits from device.
    struct
    {
        float maxAnisotropy;
        float timestampPeriod;
        vk::Format depthStencilFormat;
    }
    limits;

    //----------------------------------------------------------

    vk::DescriptorSetLayout create_descriptor_set_layout (
        ArrayProxyRef<vk::DescriptorSetLayoutBinding> bindings
    ) const;

    vk::DescriptorSetLayout create_descriptor_set_layout (
        ArrayProxyRef<vk::DescriptorSetLayoutBinding> bindings, ArrayProxyRef<vk::DescriptorBindingFlags> bindingFlags
    ) const;

    vk::PipelineLayout create_pipeline_layout (
        ArrayProxyRef<vk::DescriptorSetLayout> setLayouts, ArrayProxyRef<vk::PushConstantRange> pushConstantRanges
    ) const;

    vk::ImageView create_image_view (
        vk::Image image, vk::ImageViewType viewType, vk::Format format, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect, uint baseLevel, uint levelCount, uint baseLayer, uint layerCount
    ) const;

    vk::Sampler create_sampler (
        vk::Filter magFilter, vk::Filter minFilter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode wrapX, vk::SamplerAddressMode wrapY, vk::SamplerAddressMode wrapZ, float lodBias, uint minLod, uint maxLod, bool anisotropic, bool shadow, vk::BorderColor border
    ) const;

    vk::RenderPass create_render_pass (
        ArrayProxyRef<vk::AttachmentDescription> attachments, ArrayProxyRef<vk::SubpassDescription> subpasses, ArrayProxyRef<vk::SubpassDependency> dependencies
    ) const;

    vk::Framebuffer create_framebuffer (
        vk::RenderPass renderPass, ArrayProxyRef<vk::ImageView> imageViews, Vec2U size, uint layers
    ) const;

    vk::DescriptorSet allocate_descriptor_set (
        vk::DescriptorPool pool, vk::DescriptorSetLayout layout
    ) const;

    //----------------------------------------------------------

    /// Give an object a human readable name for debugging.
    template <class Object>
    void set_debug_object_name(Object object, const char* name) const;

private: //=====================================================

    VulkanContext(VulkanAllocator& allocator);

    static void construct(VulkanAllocator& allocator);

    static void destruct();

    static VulkanContext& get_mutable();

    // semantically, the window owns the vulkan context
    friend Window;
};

//==============================================================================

inline vk::DescriptorSetLayout VulkanContext::create_descriptor_set_layout(ArrayProxyRef<vk::DescriptorSetLayoutBinding> bindings) const
{
    return device.createDescriptorSetLayout (
        vk::DescriptorSetLayoutCreateInfo (
            {}, bindings.size(), bindings.data()
        )
    );
}

inline vk::DescriptorSetLayout VulkanContext::create_descriptor_set_layout(ArrayProxyRef<vk::DescriptorSetLayoutBinding> bindings, ArrayProxyRef<vk::DescriptorBindingFlags> bindingFlags) const
{
    return device.createDescriptorSetLayout (
        vk::StructureChain (
            vk::DescriptorSetLayoutCreateInfo (
                {}, bindings.size(), bindings.data()
            ),
            vk::DescriptorSetLayoutBindingFlagsCreateInfo (
                bindingFlags.size(), bindingFlags.data()
            )
        ).get()
    );
}

inline vk::PipelineLayout VulkanContext::create_pipeline_layout(ArrayProxyRef<vk::DescriptorSetLayout> setLayouts, ArrayProxyRef<vk::PushConstantRange> pushConstantRanges) const
{
    return device.createPipelineLayout (
        vk::PipelineLayoutCreateInfo (
            {}, setLayouts.size(), setLayouts.data(), pushConstantRanges.size(), pushConstantRanges.data()
        )
    );
}

inline vk::ImageView VulkanContext::create_image_view(vk::Image image, vk::ImageViewType viewType, vk::Format format, vk::ComponentMapping swizzle, vk::ImageAspectFlags aspect, uint baseLevel, uint levelCount, uint baseLayer, uint layerCount) const
{
    return device.createImageView (
        vk::ImageViewCreateInfo (
            {}, image, viewType, format, swizzle, vk::ImageSubresourceRange(aspect, baseLevel, levelCount, baseLayer, layerCount)
        )
    );
}

inline vk::Sampler VulkanContext::create_sampler(vk::Filter magFilter, vk::Filter minFilter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode wrapX, vk::SamplerAddressMode wrapY, vk::SamplerAddressMode wrapZ, float lodBias, uint minLod, uint maxLod, bool anisotropic, bool shadow, vk::BorderColor border) const
{
    return device.createSampler (
        vk::SamplerCreateInfo (
            {}, magFilter, minFilter, mipmapMode, wrapX, wrapY, wrapZ, lodBias, anisotropic, limits.maxAnisotropy, shadow, vk::CompareOp::eLessOrEqual, float(minLod), float(maxLod), border, false
        )
    );
}

inline vk::RenderPass VulkanContext::create_render_pass(ArrayProxyRef<vk::AttachmentDescription> attachments, ArrayProxyRef<vk::SubpassDescription> subpasses, ArrayProxyRef<vk::SubpassDependency> dependencies) const
{
    return device.createRenderPass (
        vk::RenderPassCreateInfo (
            {}, attachments.size(), attachments.data(), subpasses.size(), subpasses.data(), dependencies.size(), dependencies.data()
        )
    );
}

inline vk::Framebuffer VulkanContext::create_framebuffer(vk::RenderPass renderPass, ArrayProxyRef<vk::ImageView> imageViews, Vec2U size, uint layers) const
{
    return device.createFramebuffer (
        vk::FramebufferCreateInfo (
            {}, renderPass, imageViews.size(), imageViews.data(), size.x, size.y, layers
        )
    );
}

//==============================================================================

template <class Object>
inline void VulkanContext::set_debug_object_name(Object object [[maybe_unused]], const char* name [[maybe_unused]]) const
{
  #ifdef SQEE_DEBUG
    auto handle = reinterpret_cast<uint64_t>(static_cast<typename Object::CType>(object));
    device.setDebugUtilsObjectNameEXT({object.objectType, handle, name});
  #endif
}

} // namespace sq ##############################################################
