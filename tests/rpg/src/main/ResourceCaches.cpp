#include <sqee/gl/Textures.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Armature.hpp>
#include <sqee/objects/Sound.hpp>
#include <sqee/objects/Material.hpp>

#include "ResourceCaches.hpp"

using namespace sqt;
\
//============================================================================//

ResourceCaches::ResourceCaches()
{
    textures.assign_factory([](const std::string& key)
    {
        auto texture = sq::Texture2D::make_from_package(key);
        return std::make_unique<sq::Texture2D>(std::move(texture));
    });

    meshes.assign_factory([](const std::string& key)
    {
        auto mesh = sq::Mesh::make_from_package(key);
        return std::make_unique<sq::Mesh>(std::move(mesh));
    });

    // fixme: sounds/armatures/materials
}

ResourceCaches::~ResourceCaches() = default;
