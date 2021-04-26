#include <sqee/vk/VulkMaterial.hpp>

#include <sqee/vk/Helpers.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Json.hpp>

using namespace sq;

//============================================================================//

VulkMaterial::VulkMaterial(VulkMaterial&& other)
{
    *this = std::move(other);
}

VulkMaterial& VulkMaterial::operator=(VulkMaterial&& other)
{
    std::swap(mPipeline, other.mPipeline);
    std::swap(mTextures, other.mTextures);
    std::swap(mParamBuffer, other.mParamBuffer);
    std::swap(mParamBufferMem, other.mParamBufferMem);
    std::swap(mDescriptorSet, other.mDescriptorSet);
    return *this;
}

VulkMaterial::~VulkMaterial()
{
    const auto& ctx = VulkanContext::get();
    if (mParamBuffer) ctx.device.destroy(mParamBuffer);
    if (mParamBufferMem) mParamBufferMem.free();
    if (mDescriptorSet) ctx.device.free(ctx.descriptorPool, mDescriptorSet);
}

//============================================================================//

void VulkMaterial::load_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures)
{
    const auto& ctx = VulkanContext::get();

    mPipeline = pipelines.acquire(json.at("pipeline"));

    mDescriptorSet = vk_allocate_descriptor_set(ctx, mPipeline->get_material_set_layout());

    if (mPipeline->get_param_block_size() > 0u)
    {
        // todo: params don't change, so buffer should be on device memory
        std::tie(mParamBuffer, mParamBufferMem) = vk_create_buffer (
            ctx, mPipeline->get_param_block_size(), vk::BufferUsageFlagBits::eUniformBuffer, true
        );

        vk_update_descriptor_set (
            ctx, mDescriptorSet, 0u, 0u, vk::DescriptorType::eUniformBuffer,
            vk::DescriptorBufferInfo { mParamBuffer, 0u, mPipeline->get_param_block_size() }
        );

        std::byte* block = mParamBufferMem.map();

        for (const auto& [key, value] : json.at("params").items())
        {
            const auto info = mPipeline->get_param_info(key);
            if (!info.has_value()) log_error("pipeline does not have param '{}'", key);

            if      (info->type == "int")   value.get_to(*reinterpret_cast<int*>  (block + info->offset));
            else if (info->type == "Vec2I") value.get_to(*reinterpret_cast<Vec2I*>(block + info->offset));
            else if (info->type == "Vec3I") value.get_to(*reinterpret_cast<Vec3I*>(block + info->offset));
            else if (info->type == "Vec4I") value.get_to(*reinterpret_cast<Vec4I*>(block + info->offset));
            else if (info->type == "float") value.get_to(*reinterpret_cast<float*>(block + info->offset));
            else if (info->type == "Vec2F") value.get_to(*reinterpret_cast<Vec2F*>(block + info->offset));
            else if (info->type == "Vec3F") value.get_to(*reinterpret_cast<Vec3F*>(block + info->offset));
            else if (info->type == "Vec4F") value.get_to(*reinterpret_cast<Vec4F*>(block + info->offset));

            else log_error("unsupported type for uniform '{}'", key);
        }
    }

    for (const auto& [key, value] : json.at("textures").items())
    {
        const auto info = mPipeline->get_texture_info(key);
        if (!info.has_value()) log_error("pipeline does not have texture '{}'", key);

        if (info->type == "Texture2D")
        {
            mTextures.emplace_back(textures.acquire(value));

            vk_update_descriptor_set (
                ctx, mDescriptorSet, info->binding, 0u, vk::DescriptorType::eCombinedImageSampler,
                mTextures.back()->get_descriptor_info()
            );
        }
        else if (info->type == "TextureArray") log_error("todo: texarrays in materials");
        else if (info->type == "TextureCube") log_error("todo: texcubes in materials");

        else log_error("unsupported type for texture '{}'", key);
    }
}

//============================================================================//

void VulkMaterial::bind(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindPipeline (
        vk::PipelineBindPoint::eGraphics, mPipeline->get_pipeline()
    );
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX, mDescriptorSet, nullptr
    );
}

void VulkMaterial::bind_final_descriptor_set(vk::CommandBuffer cmdbuf, vk::DescriptorSet dset) const
{
    cmdbuf.bindDescriptorSets (
        vk::PipelineBindPoint::eGraphics, mPipeline->get_pipeline_layout(),
        Pipeline::MATERIAL_SET_INDEX + 1u, dset, nullptr
    );
}
