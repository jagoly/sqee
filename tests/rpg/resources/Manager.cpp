#include <unordered_map>

#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/sound/Sound.hpp>

#include "Material.hpp"
#include "Manager.hpp"

namespace {

using MapTexture = std::unordered_map<string, sq::Resource<sq::Texture2D>>;
MapTexture& static_MapTexture() { static MapTexture resMap; return resMap; }

using MapMesh = std::unordered_map<string, sq::Resource<sq::Mesh>>;
MapMesh& static_MapMesh() { static MapMesh resMap; return resMap; }

using MapArmature = std::unordered_map<string, sq::Resource<sq::Armature>>;
MapArmature& static_MapArmature() { static MapArmature resMap; return resMap; }

using MapSoundWave = std::unordered_map<string, sq::Resource<sq::SoundWave>>;
MapSoundWave& static_MapSoundWave() { static MapSoundWave resMap; return resMap; }

using MapMaterial = std::unordered_map<string, sq::Resource<sqt::Material>>;
MapMaterial& static_MapMaterial() { static MapMaterial resMap; return resMap; }

} // anonymous

namespace sqt {

HandleTexture acquire_Texture(const string& _path)
{
    auto& resource = static_MapTexture()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr = sq::load_Texture2D(_path);
    return resource;
}

HandleMesh acquire_Mesh(const string& _path)
{
    auto& resource = static_MapMesh()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::Mesh(_path));
    return resource;
}

HandleArmature acquire_Armature(const string& _path)
{
    auto& resource = static_MapArmature()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::Armature(_path));
    return resource;
}

HandleSoundWave acquire_SoundWave(const string& _path)
{
    auto& resource = static_MapSoundWave()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::SoundWave(_path));
    return resource;
}

HandleMaterial acquire_Material(const string& _path)
{
    auto& resource = static_MapMaterial()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new Material(_path));
    return resource;
}

} // namespace sqt
