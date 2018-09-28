#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/sound/Sound.hpp>
#include <sqee/render/Material.hpp>

#include "ResourceCaches.hpp"

using namespace sqt;

//============================================================================//

TextureCache::TextureCache() = default;

MeshCache::MeshCache() = default;

ArmatureCache::ArmatureCache() = default;

SoundCache::SoundCache() = default;

MaterialCache::MaterialCache(TextureCache& tc) : mTextureCache(tc) {}

//============================================================================//

TextureCache::~TextureCache() = default;

MeshCache::~MeshCache() = default;

ArmatureCache::~ArmatureCache() = default;

SoundCache::~SoundCache() = default;

MaterialCache::~MaterialCache() = default;

//============================================================================//

UniquePtr<sq::Texture2D> TextureCache::create(const String& path)
{
    auto texture = sq::Texture2D::make_from_package(path);
    return std::make_unique<sq::Texture2D>(std::move(texture));
}

UniquePtr<sq::Mesh> MeshCache::create(const String& path)
{
    return sq::Mesh::make_from_package(path);
}

UniquePtr<sq::Armature> ArmatureCache::create(const String& path)
{
    return {};
}

UniquePtr<sq::SoundWave> SoundCache::create(const String& path)
{
    return std::make_unique<sq::SoundWave>(path);
}

UniquePtr<sq::Material> MaterialCache::create(const String& path)
{
    return std::make_unique<sq::Material>(path, mTextureCache);
}

//============================================================================//

ResourceCaches::ResourceCaches()
    : textures(), meshes(), armatures(), sounds(),
      materials(textures) {}

ResourceCaches::~ResourceCaches() = default;
