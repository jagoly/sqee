#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/Resources.hpp>

// Forward Declarations /////
namespace sq { class Armature; class Texture2D; class Mesh; }
namespace sqt { class Material; }

namespace sqt {

using HandleMaterial = sq::Handle<Material>;
using HandleArmature = sq::Handle<sq::Armature>;
using HandleTexture = sq::Handle<sq::Texture2D>;
using HandleMesh = sq::Handle<sq::Mesh>;

HandleMaterial acquire_Material(const string& _path);
HandleArmature acquire_Armature(const string& _path);
HandleTexture acquire_Texture(const string& _path);
HandleMesh acquire_Mesh(const string& _path);

}
