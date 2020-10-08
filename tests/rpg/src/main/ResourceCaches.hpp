#pragma once

#include <sqee/misc/Json.hpp>
#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>

//====== Forward Declarations ================================================//

namespace sq {

class Texture2D;
class Mesh;
class Armature;
class Sound;
class Material;

} // namespace sq

//============================================================================//

namespace sqt {

//====== Alias Declarations ==================================================//

using TextureHandle  = sq::Handle<std::string, sq::Texture2D>;
using MeshHandle     = sq::Handle<std::string, sq::Mesh>;
using ArmatureHandle = sq::Handle<std::string, sq::Armature>;
using SoundHandle    = sq::Handle<std::string, sq::Sound>;
using MaterialHandle = sq::Handle<sq::JsonValue, sq::Material>;

//============================================================================//

class ResourceCaches final : sq::NonCopyable
{
public: //====================================================//

    ResourceCaches();
    ~ResourceCaches();

    //--------------------------------------------------------//

    sq::ResourceCache<std::string, sq::Texture2D>  textures;
    sq::ResourceCache<std::string, sq::Mesh>       meshes;
    sq::ResourceCache<std::string, sq::Armature>   armatures;
    sq::ResourceCache<std::string, sq::Sound>      sounds;
    sq::ResourceCache<sq::JsonValue, sq::Material> materials;
};

//============================================================================//

} // namespace sqt
