#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/VulkanMemory.hpp>
#include <sqee/vk/PassConfig.hpp>

namespace sq {

//============================================================================//

class SQEE_API Pipeline : private MoveOnly
{
public: //====================================================//

    // shader ubos/samplers are expected to be as follows:
    // - set 0 contains data shared between passes, like camera info and global options
    // - set 1 contains data specific to the pass, like light info and special textures
    // - set 2 contains material data, and is loaded from json
    // - set 3 contains data for the object being drawn, like model mat or bones

    static constexpr uint DESCRIPTOR_SET_COUNT = 4u;
    static constexpr uint MATERIAL_SET_INDEX = 2u;

    //--------------------------------------------------------//

    struct ParamInfo
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

    void load_from_json(const JsonValue& json, const PassConfigMap& passes);

    //--------------------------------------------------------//

    vk::DescriptorSetLayout get_material_set_layout() const { return mMaterialSetLayout; }

    vk::PipelineLayout get_pipeline_layout() const { return mPipelineLayout; }

    vk::Pipeline get_pipeline() const { return mPipeline; }

    size_t get_param_block_size() const { return mParamBlockSize; }

    //--------------------------------------------------------//

    std::optional<ParamInfo> get_param_info(TinyString name) const
    {
        const auto iter = mParamMap.find(name);
        if (iter != mParamMap.end()) return iter->second;
        return std::nullopt;
    }

    std::optional<TextureInfo> get_texture_info(TinyString name) const
    {
        const auto iter = mTextureMap.find(name);
        if (iter != mTextureMap.end()) return iter->second;
        return std::nullopt;
    }

protected: //=================================================//

    vk::DescriptorSetLayout mMaterialSetLayout;
    vk::PipelineLayout mPipelineLayout;

    vk::Pipeline mPipeline;

    std::map<TinyString, ParamInfo> mParamMap;
    std::map<TinyString, TextureInfo> mTextureMap;

    size_t mParamBlockSize = 0u;
};

//============================================================================//

} // namespace sq
