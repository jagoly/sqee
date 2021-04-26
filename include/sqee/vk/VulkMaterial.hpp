// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/vk/Pipeline.hpp>
#include <sqee/vk/VulkTexture.hpp>

#include <sqee/misc/ResourceCache.hpp>

namespace sq {

//============================================================================//

/// The SQEE Material class.
class SQEE_API VulkMaterial final : private MoveOnly
{
public: //====================================================//

    using PipelineCache = ResourceCache<JsonValue, Pipeline>;
    using TextureCache = ResourceCache<String, VulkTexture>;

    //--------------------------------------------------------//

    VulkMaterial() = default;

    VulkMaterial(VulkMaterial&& other);
    VulkMaterial& operator=(VulkMaterial&& other);

    ~VulkMaterial();

    //--------------------------------------------------------//

    /// Load the material from a json object.
    void load_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures);

    /// Bind the material to the context.
    void bind(vk::CommandBuffer cmdbuf) const;

    /// Bind the per object descriptor set.
    void bind_final_descriptor_set(vk::CommandBuffer cmdbuf, vk::DescriptorSet dset) const;

private: //===================================================//

    Handle<JsonValue, Pipeline> mPipeline;
    std::vector<Handle<String, VulkTexture>> mTextures;

    vk::Buffer mParamBuffer;
    VulkanMemory mParamBufferMem;

    vk::DescriptorSet mDescriptorSet;
};

//============================================================================//

} // namespace sq
