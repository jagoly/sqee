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

    struct Config2D
    {
        vk::Format format;
        vk::SamplerAddressMode wrapX;
        vk::SamplerAddressMode wrapY;
        vk::ComponentMapping swizzle;
        bool filter;
        bool mipmaps;
        bool anisotropy;
    };

    //--------------------------------------------------------//

    VulkTexture() = default;

    VulkTexture(VulkTexture&& other);
    VulkTexture& operator=(VulkTexture&& other);

    ~VulkTexture();

    //--------------------------------------------------------//

    void load_from_memory_2D(void* data, Vec2U size, Config2D config);

    void load_from_file_2D(const String& path);

    //--------------------------------------------------------//

    const vk::DescriptorImageInfo& get_descriptor_info() const { return mDescriptorInfo; }

protected: //=================================================//

    vk::Image mImage;
    VulkanMemory mImageMem;
    vk::ImageView mImageView;
    vk::Sampler mSampler;

    vk::DescriptorImageInfo mDescriptorInfo;
};

//============================================================================//

} // namespace sq
