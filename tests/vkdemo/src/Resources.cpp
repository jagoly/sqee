#include "Resources.hpp"

#include <sqee/misc/Json.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/vk/VulkanContext.hpp>

using namespace demo;

//============================================================================//

ResourceCaches::ResourceCaches()
{
    const auto& ctx = sq::VulkanContext::get();

    bindlessTextureSetLayout = ctx.create_descriptor_set_layout (
        vk::DescriptorSetLayoutBinding(0u, vk::DescriptorType::eCombinedImageSampler, 16u, vk::ShaderStageFlagBits::eFragment),
        vk::Flags(vk::DescriptorBindingFlagBits::ePartiallyBound)
    );

    bindlessTextureSet = ctx.allocate_descriptor_set(ctx.descriptorPool, bindlessTextureSetLayout);

    //-- Assign Factory Functions ----------------------------//

    meshes.assign_factory([](const String& key)
    {
        auto result = sq::Mesh();
        result.load_from_file("assets/" + key);
        return result;
    });

    textures.assign_factory([this](const String& key)
    {
        auto result = sq::Texture();
        result.load_from_file_2D("assets/" + key);
        result.add_to_bindless_descriptor_set(bindlessTextureSet, numBindlessTextures);
        ++numBindlessTextures;
        return result;
    });

    pipelines.assign_factory([this](const String& key)
    {
        auto result = sq::Pipeline();
        result.load_from_minified_json(key, passConfigMap);
        return result;
    });
}

ResourceCaches::~ResourceCaches()
{
    const auto& ctx = sq::VulkanContext::get();

    ctx.device.destroy(bindlessTextureSetLayout);
    ctx.device.free(ctx.descriptorPool, bindlessTextureSet);
}
