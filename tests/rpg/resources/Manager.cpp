#include <unordered_map>

#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Armature.hpp>

#include "Material.hpp"
#include "Manager.hpp"

using namespace sqt;

using MapMaterial = std::unordered_map<string, sq::Resource<Material>>;
using MapArmature = std::unordered_map<string, sq::Resource<sq::Armature>>;
using MapTexture = std::unordered_map<string, sq::Resource<sq::Texture2D>>;
using MapMesh = std::unordered_map<string, sq::Resource<sq::Mesh>>;

MapMaterial& static_MapMaterial() { static MapMaterial resMap; return resMap; }
MapArmature& static_MapArmature() { static MapArmature resMap; return resMap; }
MapTexture& static_MapTexture() { static MapTexture resMap; return resMap; }
MapMesh& static_MapMesh() { static MapMesh resMap; return resMap; }

HandleMaterial sqt::acquire_Material(const string& _path) {
    auto& resource = static_MapMaterial()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new Material(_path));
    return resource;
}

HandleArmature sqt::acquire_Armature(const string& _path) {
    auto& resource = static_MapArmature()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::Armature(_path));
    return resource;
}

HandleTexture sqt::acquire_Texture(const string& _path) {
    auto& resource = static_MapTexture()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr = sq::load_Texture2D(_path);
    return resource;
}

HandleMesh sqt::acquire_Mesh(const string& _path) {
    auto& resource = static_MapMesh()[_path];
    if (resource.loaded() == true) return resource;
    resource.uptr.reset(new sq::Mesh(_path));
    return resource;
}
