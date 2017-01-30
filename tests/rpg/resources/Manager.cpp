#include <unordered_map>

#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/sound/Sound.hpp>

#include "Manager.hpp"

namespace { // anonymous

using MapTexture = std::unordered_map<string, sq::Resource<sq::Texture2D>>;
MapTexture& static_MapTexture() { static MapTexture resMap; return resMap; }

using MapMesh = std::unordered_map<string, sq::Resource<sq::Mesh>>;
MapMesh& static_MapMesh() { static MapMesh resMap; return resMap; }

using MapArmature = std::unordered_map<string, sq::Resource<sq::Armature>>;
MapArmature& static_MapArmature() { static MapArmature resMap; return resMap; }

using MapSoundWave = std::unordered_map<string, sq::Resource<sq::SoundWave>>;
MapSoundWave& static_MapSoundWave() { static MapSoundWave resMap; return resMap; }

using MapMaterial = std::unordered_map<string, sq::Resource<sq::Material>>;
MapMaterial& static_MapMaterial() { static MapMaterial resMap; return resMap; }

} // anonymous namespace

namespace sqt {

//========================================================//

HandleTexture acquire_Texture(const string& path)
{
    auto& resource = static_MapTexture()[path];
    if (resource.loaded() == true) return resource;

    auto texture = sq::Texture2D::make_from_package(path);
    resource.uptr = std::make_unique<sq::Texture2D>(std::move(texture));

    return resource;
}

//========================================================//

HandleMesh acquire_Mesh(const string& path)
{
    auto& resource = static_MapMesh()[path];
    if (resource.loaded()) return resource;
    resource.uptr = sq::Mesh::make_from_package(path);
    return resource;
}

//========================================================//

//HandleArmature acquire_Armature(const string& path)
//{
//    auto& resource = static_MapArmature()[path];
//    if (resource.loaded() == true) return resource;
//    resource.uptr.reset(new sq::Armature(path));
//    return resource;
//}

//========================================================//

HandleSoundWave acquire_SoundWave(const string& path)
{
    auto& resource = static_MapSoundWave()[path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::SoundWave(path));
    return resource;
}

//========================================================//

HandleMaterial acquire_Material(const string& path)
{
    auto& resource = static_MapMaterial()[path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::Material(path, &acquire_Texture));
    return resource;
}

//========================================================//

} // namespace sqt
