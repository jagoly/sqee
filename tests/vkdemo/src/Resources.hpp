#pragma once

#include "setup.hpp"

#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>

#include <sqee/vk/VulkMesh.hpp>
#include <sqee/vk/VulkTexture.hpp>
#include <sqee/vk/Pipeline.hpp>
#include <sqee/vk/VulkMaterial.hpp>

namespace demo {

using MeshCache = sq::ResourceCache<String, sq::VulkMesh>;
using TextureCache = sq::ResourceCache<String, sq::VulkTexture>;
using PipelineCache = sq::ResourceCache<JsonValue, sq::Pipeline>;
using MaterialCache = sq::ResourceCache<JsonValue, sq::VulkMaterial>;

using MeshHandle = sq::Handle<String, sq::VulkMesh>;
using TextureHandle = sq::Handle<String, sq::VulkTexture>;
using PipelineHandle = sq::Handle<JsonValue, sq::Pipeline>;
using MaterialHandle = sq::Handle<JsonValue, sq::VulkMaterial>;

struct ResourceCaches final : sq::NonCopyable
{
    ResourceCaches();
    ~ResourceCaches();

    MeshCache meshes;
    TextureCache textures;
    PipelineCache pipelines;
    MaterialCache materials;

    sq::PassConfigMap passConfigMap;
};

} // namespace demo
