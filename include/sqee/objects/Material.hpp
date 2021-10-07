// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

#include <sqee/misc/ResourceCache.hpp>

#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/Wrappers.hpp>

namespace sq {

//============================================================================//

/// Information needed to render objects, defined in JSON.
///
/// Each material consists of one or more passes.
/// Each pass has a Pipeline Handle, a set of Texture Handles, and a Uniform Buffer.
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

    /// Bind the material descriptor set of the specified pass.
    void bind_material_set(vk::CommandBuffer cmdbuf, uint8_t pass) const;

    /// Bind an object descriptor set for the specified pass.
    void bind_object_set(vk::CommandBuffer cmdbuf, uint8_t pass, vk::DescriptorSet objectSet) const;

    /// Access the pipeline resource of the specified pass.
    const Pipeline& get_pipeline(uint8_t pass) const { return mPasses[pass].pipeline.get(); }

    /// Get the number of material passes.
    uint8_t get_pass_count() const { return uint8_t(mPasses.size()); }

private: //===================================================//

    void impl_load_pass_from_json(const JsonValue& json, PipelineCache& pipelines, TextureCache& textures);

    struct MaterialPass
    {
        Handle<JsonValue, Pipeline> pipeline;
        std::vector<Handle<String, Texture>> textures;
        BufferStuff paramBuffer;
        vk::DescriptorSet descriptorSet;
    };

    std::vector<MaterialPass> mPasses;
};

//============================================================================//

} // namespace sq
