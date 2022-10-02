#pragma once

#include "setup.hpp"

#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>

#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

namespace demo {

using MeshCache = sq::ResourceCache<String, sq::Mesh>;
using TextureCache = sq::ResourceCache<String, sq::Texture>;
using PipelineCache = sq::ResourceCache<JsonValue, sq::Pipeline>;

using MeshHandle = sq::Handle<String, sq::Mesh>;
using TextureHandle = sq::Handle<String, sq::Texture>;
using PipelineHandle = sq::Handle<JsonValue, sq::Pipeline>;

struct ResourceCaches final
{
    ResourceCaches();

    SQEE_COPY_DELETE(ResourceCaches)
    SQEE_MOVE_DELETE(ResourceCaches)

    ~ResourceCaches();

    MeshCache meshes;
    TextureCache textures;
    PipelineCache pipelines;

    sq::PassConfigMap passConfigMap;

    vk::DescriptorSetLayout bindlessTextureSetLayout;
    vk::DescriptorSet bindlessTextureSet;

    uint32_t numBindlessTextures = 0u;
};

} // namespace demo
