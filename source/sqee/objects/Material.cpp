#include <sqee/objects/Material.hpp>

#include <sqee/vk/Helpers.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Json.hpp>

using namespace sq;

//============================================================================//

Material::Material(Material&& other)
{
    *this = std::move(other);
}

Material& Material::operator=(Material&& other)
{
    mPipeline = std::move(other.mPipeline);
    mTextures = std::move(other.mTextures);
    std::swap(mParamBuffer, other.mParamBuffer);
    std::swap(mDescriptorSet, other.mDescriptorSet);
    return *this;
}

Material::~Material()
{
    const auto& ctx = VulkanContext::get();
    if (mParamBuffer.buffer) mParamBuffer.destroy(ctx);
    if (mDescriptorSet) ctx.device.free(ctx.descriptorPool, mDescriptorSet);
}

//============================================================================//

void Material::load_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures)
{
    SQASSERT(!mDescriptorSet, "material already loaded");

    const auto& ctx = VulkanContext::get();

    mPipeline = pipelines.acquire(json.at("pipeline"));

    mDescriptorSet = vk_allocate_descriptor_set(ctx, mPipeline->get_material_set_layout());

    if (mPipeline->get_material_param_block_size() > 0u)
    {
        // todo: params don't change, so buffer should be on device memory
        mParamBuffer.initialise (
            ctx, mPipeline->get_material_param_block_size(), vk::BufferUsageFlagBits::eUniformBuffer, true
        );

        vk_update_descriptor_set (
            ctx, mDescriptorSet, DescriptorUniformBuffer(0u, 0u, mParamBuffer.buffer, 0u, mPipeline->get_material_param_block_size())
        );

        std::byte* block = mParamBuffer.memory.map();

        for (const auto& [key, value] : json.at("params").items())
        {
            const auto info = mPipeline->get_material_param_info(key);
            if (!info.has_value()) SQEE_THROW("pipeline does not have param '{}'", key);

            if      (info->type == "int")   value.get_to(*reinterpret_cast<int*>  (block + info->offset));
            else if (info->type == "Vec2I") value.get_to(*reinterpret_cast<Vec2I*>(block + info->offset));
            else if (info->type == "Vec3I") value.get_to(*reinterpret_cast<Vec3I*>(block + info->offset));
            else if (info->type == "Vec4I") value.get_to(*reinterpret_cast<Vec4I*>(block + info->offset));
            else if (info->type == "float") value.get_to(*reinterpret_cast<float*>(block + info->offset));
            else if (info->type == "Vec2F") value.get_to(*reinterpret_cast<Vec2F*>(block + info->offset));
            else if (info->type == "Vec3F") value.get_to(*reinterpret_cast<Vec3F*>(block + info->offset));
            else if (info->type == "Vec4F") value.get_to(*reinterpret_cast<Vec4F*>(block + info->offset));

            else SQEE_THROW("unsupported type for uniform '{}'", key);
        }
    }
    else if (!json.at("params").empty()) SQEE_THROW("material params given, but pipeline has none");

    for (const auto& [key, value] : json.at("textures").items())
    {
        const auto info = mPipeline->get_texture_info(key);
        if (!info.has_value()) SQEE_THROW("pipeline does not have texture '{}'", key);

        if (info->type == "Texture2D")
        {
            mTextures.emplace_back(textures.acquire(value));

            vk_update_descriptor_set (
                ctx, mDescriptorSet, DescriptorImageSampler(info->binding, 0u, mTextures.back()->get_descriptor_info())
            );
        }
        else if (info->type == "TextureArray") SQEE_THROW("todo: texarrays in materials");
        else if (info->type == "TextureCube") SQEE_THROW("todo: texcubes in materials");

        else SQEE_THROW("unsupported type for texture '{}'", key);
    }
}

//============================================================================//

void Material::bind_material_set(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX, mDescriptorSet, nullptr
    );
}

void Material::bind_object_set(vk::CommandBuffer cmdbuf, vk::DescriptorSet dset) const
{
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX + 1u, dset, nullptr
    );
}
