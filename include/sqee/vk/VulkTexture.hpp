#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

//============================================================================//

class SQEE_API VulkTexture : private MoveOnly
{
public: //====================================================//

    VulkTexture() = default;

    VulkTexture(VulkTexture&& other);
    VulkTexture& operator=(VulkTexture&& other);

    ~VulkTexture();

    //--------------------------------------------------------//

    void load_automatic_2D(const VulkanContext& ctx, const String& path);

    //--------------------------------------------------------//

    const vk::DescriptorImageInfo& get_descriptor_info() const { return mDescriptorInfo; }

protected: //=================================================//

    const VulkanContext* ctx = nullptr;

    vk::Image mImage;
    VulkanMemory mImageMem;
    vk::ImageView mImageView;
    vk::Sampler mSampler;

    vk::DescriptorImageInfo mDescriptorInfo;
};

//============================================================================//

} // namespace sq
