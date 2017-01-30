#pragma once

#include <sqee/builtins.hpp>
#include <sqee/misc/Resource.hpp>

// Forward Declarations /////
namespace sq { class Texture2D; class Mesh; class Material; class SoundWave; class Armature; }

namespace sqt {

//============================================================================//

using HandleTexture = sq::Handle<sq::Texture2D>;
HandleTexture acquire_Texture(const string& path);

using HandleMesh = sq::Handle<sq::Mesh>;
HandleMesh acquire_Mesh(const string& path);

using HandleArmature = sq::Handle<sq::Armature>;
//HandleArmature acquire_Armature(const string& path);

using HandleSoundWave = sq::Handle<sq::SoundWave>;
HandleSoundWave acquire_SoundWave(const string& path);

using HandleMaterial = sq::Handle<sq::Material>;
HandleMaterial acquire_Material(const string& path);

//============================================================================//

} // namespace sqt
