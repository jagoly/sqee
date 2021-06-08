#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>

namespace sq {

//============================================================================//

/// Image and Sampler, with metadata loaded from JSON.
///
/// Supports 2D, Array, and Cube textures,
/// Will automatically generate mipmaps if requested.
///
class SQEE_API Texture : private MoveOnly
{
public: //====================================================//

    enum class FilterMode : int8_t { Nearest, Linear, Anisotropic };

    enum class MipmapsMode : int8_t { Disable, Generate, Load };

    struct Config
    {
        vk::Format format;
        vk::SamplerAddressMode wrapX;
        vk::SamplerAddressMode wrapY;
        vk::SamplerAddressMode wrapZ;
        vk::ComponentMapping swizzle;
        FilterMode filter;
        MipmapsMode mipmaps;
        Vec3U size;
        uint mipLevels;
    };

    //--------------------------------------------------------//

    Texture() = default;

    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    ~Texture();

    //--------------------------------------------------------//

    void initialise_2D(const Config& config);

    void load_from_file_2D(const String& path);

    //--------------------------------------------------------//

    void initialise_array(const Config& config);

    void load_from_file_array(const String& path);

    //--------------------------------------------------------//

    void initialise_cube(const Config& config);

    void load_from_file_cube(const String& path);

    //--------------------------------------------------------//

    void load_from_memory(void* data, uint level, uint layer, const Config& config);

    //--------------------------------------------------------//

    const vk::DescriptorImageInfo& get_descriptor_info() const { return mDescriptorInfo; }

protected: //=================================================//

    void impl_initialise_common(const Config& config);

    vk::Image mImage;
    VulkanMemory mImageMem;
    vk::ImageView mImageView;
    vk::Sampler mSampler;

    vk::DescriptorImageInfo mDescriptorInfo;
};

//============================================================================//

} // namespace sq
