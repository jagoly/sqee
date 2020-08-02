#include <sqee/gl/Textures.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Armature.hpp>
#include <sqee/objects/Sound.hpp>
#include <sqee/objects/Material.hpp>

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

std::unique_ptr<sq::Texture2D> TextureCache::create(const std::string& path)
{
    auto texture = sq::Texture2D::make_from_package(path);
    return std::make_unique<sq::Texture2D>(std::move(texture));
}

std::unique_ptr<sq::Mesh> MeshCache::create(const std::string& path)
{
    return sq::Mesh::make_from_package(path);
}

std::unique_ptr<sq::Armature> ArmatureCache::create(const std::string& /*path*/)
{
    return {};
}

std::unique_ptr<sq::SoundWave> SoundCache::create(const std::string& path)
{
    return std::make_unique<sq::SoundWave>(path);
}

std::unique_ptr<sq::Material> MaterialCache::create(const std::string& path)
{
    return std::make_unique<sq::Material>(path, mTextureCache);
}

//============================================================================//

ResourceCaches::ResourceCaches()
    : textures(), meshes(), armatures(), sounds(),
      materials(textures) {}

ResourceCaches::~ResourceCaches() = default;
