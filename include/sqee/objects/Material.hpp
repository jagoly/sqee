// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

#include <sqee/misc/ResourceCache.hpp>

namespace sq {

//============================================================================//

/// Information needed to render objects, defined in JSON.
///
/// Contains a Pipeline Handle, a set of Texture Handles, and a Uniform Buffer.
/// Remember to delete your Material cache before your Pipeline and Texture caches.
///
class SQEE_API Material final : private MoveOnly
{
public: //====================================================//

    using PipelineCache = ResourceCache<JsonValue, Pipeline>;
    using TextureCache = ResourceCache<String, Texture>;

    //--------------------------------------------------------//

    Material() = default;

    Material(Material&& other);
    Material& operator=(Material&& other);

    ~Material();

    //--------------------------------------------------------//

    /// Load the material from a json object.
    void load_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures);

    /// Bind the material to the context.
    void bind(vk::CommandBuffer cmdbuf) const;

    /// Bind the per object descriptor set.
    void bind_final_descriptor_set(vk::CommandBuffer cmdbuf, vk::DescriptorSet dset) const;

    /// Access the pipeline resource.
    const Pipeline& get_pipeline() const { return *mPipeline; }

private: //===================================================//

    Handle<JsonValue, Pipeline> mPipeline;
    std::vector<Handle<String, Texture>> mTextures;

    vk::Buffer mParamBuffer;
    VulkanMemory mParamBufferMem;

    vk::DescriptorSet mDescriptorSet;
};

//============================================================================//

} // namespace sq
