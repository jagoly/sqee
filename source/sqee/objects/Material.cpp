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
    std::swap(mPasses, other.mPasses);
    return *this;
}

Material::~Material()
{
    if (mPasses.empty()) return;
    const auto& ctx = VulkanContext::get();
    for (MaterialPass& pass : mPasses)
    {
        // have to check here in case loading threw an exception before creating things
        if (pass.paramBuffer.buffer) pass.paramBuffer.destroy(ctx);
        if (pass.descriptorSet) ctx.device.free(ctx.descriptorPool, pass.descriptorSet);
    }
}

//============================================================================//

void Material::load_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures)
{
    SQASSERT(mPasses.empty(), "material already loaded");

    if (json.is_object() == true)
        impl_load_pass_from_json(json, pipelines, textures);

    else for (const JsonValue& jsonObject : json)
        impl_load_pass_from_json(jsonObject, pipelines, textures);
}

//============================================================================//

void Material::impl_load_pass_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures)
{
    const auto& ctx = VulkanContext::get();

    MaterialPass& pass = mPasses.emplace_back();

    pass.pipeline = pipelines.acquire(json.at("pipeline"));

    if (bool(pass.pipeline->get_pass_config()->renderPass) == false)
        return; // disabled render pass

    pass.descriptorSet = vk_allocate_descriptor_set(ctx, pass.pipeline->get_material_set_layout());

    if (pass.pipeline->get_material_param_block_size() > 0u)
    {
        // todo: params don't change, so buffer should be on device memory
        pass.paramBuffer.initialise (
            ctx, pass.pipeline->get_material_param_block_size(), vk::BufferUsageFlagBits::eUniformBuffer, true
        );

        vk_update_descriptor_set (
            ctx, pass.descriptorSet,
            DescriptorUniformBuffer(0u, 0u, pass.paramBuffer.buffer, 0u, pass.pipeline->get_material_param_block_size())
        );

        std::byte* block = pass.paramBuffer.memory.map();

        for (const auto& [key, value] : json.at("params").items())
        {
            const auto info = pass.pipeline->get_material_param_info(key);
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
        const auto info = pass.pipeline->get_texture_info(key);
        if (!info.has_value()) SQEE_THROW("pipeline does not have texture '{}'", key);

        if (info->type == "Texture2D")
        {
            pass.textures.emplace_back(textures.acquire(value.get_ref<const String&>()));

            vk_update_descriptor_set (
                ctx, pass.descriptorSet,
                DescriptorImageSampler(info->binding, 0u, pass.textures.back()->get_descriptor_info())
            );
        }
        else if (info->type == "TextureArray") SQEE_THROW("todo: texarrays in materials");
        else if (info->type == "TextureCube") SQEE_THROW("todo: texcubes in materials");

        else SQEE_THROW("unsupported type for texture '{}'", key);
    }
}

//============================================================================//

void Material::bind_material_set(vk::CommandBuffer cmdbuf, uint8_t pass) const
{
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPasses[pass].pipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX, mPasses[pass].descriptorSet, nullptr
    );
}

void Material::bind_object_set(vk::CommandBuffer cmdbuf, uint8_t pass, vk::DescriptorSet objectSet) const
{
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPasses[pass].pipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX + 1u, objectSet, nullptr
    );
}
