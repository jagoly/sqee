#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/PassConfig.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

/// A Vulkan Pipeline defined in JSON, with reflection data.
///
/// Shader descriptor sets are expected to be as follows:
///  0: shared data, like camera and world info
///  1: material data, loaded from JSON by Material
///  2: object data, like model matrix and bones
///
class SQEE_API Pipeline : private MoveOnly
{
public: //====================================================//

    static constexpr uint DESCRIPTOR_SET_COUNT = 3u;
    static constexpr uint MATERIAL_SET_INDEX = 1u;

    //--------------------------------------------------------//

    //struct PushConstantInfo
    //{
    //    TinyString type;
    //    vk::ShaderStageFlags stages;
    //    uint offset;
    //};

    struct MaterialParamInfo
    {
        TinyString type;
        uint offset;
    };

    struct TextureInfo
    {
        TinyString type;
        uint binding;
    };

    //--------------------------------------------------------//

    Pipeline() = default;

    Pipeline(Pipeline&& other);
    Pipeline& operator=(Pipeline&& other);

    ~Pipeline();

    //--------------------------------------------------------//

    /// Load the pipeline from a json object.
    void load_from_json(const JsonValue& json, const PassConfigMap& passes);

    /// Bind the pipeline for rendering.
    void bind(vk::CommandBuffer cmdbuf) const;

    //--------------------------------------------------------//

    const PassConfig* get_pass_config() const { return mPassConfig; }

    vk::DescriptorSetLayout get_material_set_layout() const { return mMaterialSetLayout; }

    vk::PipelineLayout get_pipeline_layout() const { return mPipelineLayout; }

    vk::Pipeline get_pipeline() const { return mPipeline; }

    size_t get_material_param_block_size() const { return mMaterialParamBlockSize; }

    //--------------------------------------------------------//

    //std::optional<PushConstantInfo> get_push_constant_info(TinyString name) const
    //{
    //    const auto iter = mPushConstantMap.find(name);
    //    if (iter != mPushConstantMap.end()) return iter->second;
    //    return std::nullopt;
    //}

    std::optional<MaterialParamInfo> get_material_param_info(TinyString name) const
    {
        const auto iter = mMaterialParamMap.find(name);
        if (iter != mMaterialParamMap.end()) return iter->second;
        return std::nullopt;
    }

    std::optional<TextureInfo> get_texture_info(TinyString name) const
    {
        const auto iter = mTextureMap.find(name);
        if (iter != mTextureMap.end()) return iter->second;
        return std::nullopt;
    }

protected: //=================================================//

    const PassConfig* mPassConfig = nullptr;

    vk::DescriptorSetLayout mMaterialSetLayout;
    vk::PipelineLayout mPipelineLayout;

    vk::Pipeline mPipeline;

    //std::map<TinyString, PushConstantInfo> mPushConstantMap;
    std::map<TinyString, MaterialParamInfo> mMaterialParamMap;
    std::map<TinyString, TextureInfo> mTextureMap;

    size_t mMaterialParamBlockSize = 0u;
};

//============================================================================//

} // namespace sq
