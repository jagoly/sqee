#pragma once

#include "setup.hpp"

#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>

#include <sqee/objects/Material.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

namespace demo {

using MeshCache = sq::ResourceCache<String, sq::Mesh>;
using TextureCache = sq::ResourceCache<String, sq::Texture>;
using PipelineCache = sq::ResourceCache<JsonValue, sq::Pipeline>;
using MaterialCache = sq::ResourceCache<JsonValue, sq::Material>;

using MeshHandle = sq::Handle<String, sq::Mesh>;
using TextureHandle = sq::Handle<String, sq::Texture>;
using PipelineHandle = sq::Handle<JsonValue, sq::Pipeline>;
using MaterialHandle = sq::Handle<JsonValue, sq::Material>;

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
