#pragma once

#include <sqee/builtins.hpp>
#include <sqee/misc/ResHolder.hpp>

// Forward Declarations /////
namespace sq { class Texture2D; class PhysObject; class Armature; class Mesh; class Skin; }

namespace sq {

string& static_path();

ResHolder<Texture2D>& static_Texture2D();
ResHolder<PhysObject>& static_PhysObject();
ResHolder<Armature>& static_Armature();
ResHolder<Mesh>& static_Mesh();
ResHolder<Skin>& static_Skin();

}
