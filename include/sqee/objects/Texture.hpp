#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/core/Utilities.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/Wrappers.hpp>

namespace sq {

//============================================================================//

/// Image and Sampler, with metadata loaded from JSON.
///
/// Supports 2D, Array, and Cube textures.
/// Will automatically generate mipmaps if requested.
///
class SQEE_API Texture : MoveOnly
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

    /// Upload data to one layer of one level of the image. Will generate mipmaps if requested.
    void load_from_memory(const void* data, size_t length, uint level, uint layer, const Config& config);

    /// Upload the entire image's data all at once.
    void load_from_memory(const void* data, size_t length, const Config& config);

    //--------------------------------------------------------//

    /// Load image data from a compressed binary file. Return false if file does not exist.
    bool try_load_from_compressed(const String& path, const Config& config);

    /// Save image data to a binary file compressed with lz4.
    void save_as_compressed(const String& path, vk::Format format, Vec3U size, uint mipLevels);

    //--------------------------------------------------------//

    /// Get info needed to update a descriptor set.
    vk::DescriptorImageInfo get_descriptor_info() const
    {
        return { mSampler, mStuff.view, vk::ImageLayout::eShaderReadOnlyOptimal };
    }

    vk::Image get_image() const { return mStuff.image; }

    vk::Sampler get_sampler() const { return mSampler; }

    void add_to_bindless_descriptor_set(vk::DescriptorSet dset, uint32_t index);

    std::optional<uint32_t> get_bindless_descriptor_index() const { return mBindlessDescriptorIndex; }

    //--------------------------------------------------------//

    /// Compute the total size of a texture, in bytes.
    static size_t compute_buffer_size(Vec3U size, uint mipLevels, size_t pixelSize);

protected: //=================================================//

    ImageStuff mStuff;
    vk::Sampler mSampler;

    std::optional<uint32_t> mBindlessDescriptorIndex;
};

//============================================================================//

} // namespace sq
