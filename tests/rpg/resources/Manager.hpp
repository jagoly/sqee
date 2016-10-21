#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/Resources.hpp>

// Forward Declarations /////
namespace sq { class Texture2D; class Mesh; class SoundWave; class Armature; }
namespace sqt { class Material; }

namespace sqt {

using HandleTexture = sq::Handle<sq::Texture2D>;
HandleTexture acquire_Texture(const string& _path);

using HandleMesh = sq::Handle<sq::Mesh>;
HandleMesh acquire_Mesh(const string& _path);

using HandleArmature = sq::Handle<sq::Armature>;
HandleArmature acquire_Armature(const string& _path);

using HandleSoundWave = sq::Handle<sq::SoundWave>;
HandleSoundWave acquire_SoundWave(const string& _path);

using HandleMaterial = sq::Handle<Material>;
HandleMaterial acquire_Material(const string& _path);

}
