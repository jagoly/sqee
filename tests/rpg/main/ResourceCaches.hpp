#pragma once

#include <sqee/misc/Builtins.hpp>

#include <sqee/misc/ResourceCache.hpp>
#include <sqee/misc/ResourceHandle.hpp>

//====== Forward Declarations ================================================//

namespace sq {

class Texture2D;
class Mesh;
class Armature;
class SoundWave;
class Material;

} // namespace sq

//============================================================================//

namespace sqt {

//====== Alias Declarations ==================================================//

using TextureHandle  = sq::Handle<String, sq::Texture2D>;
using MeshHandle     = sq::Handle<String, sq::Mesh>;
using ArmatureHandle = sq::Handle<String, sq::Armature>;
using SoundHandle    = sq::Handle<String, sq::SoundWave>;
using MaterialHandle = sq::Handle<String, sq::Material>;

//============================================================================//

class TextureCache final : public sq::ResourceCache<String, sq::Texture2D>
{
    public:  TextureCache(); ~TextureCache() override;
    private: UniquePtr<sq::Texture2D> create(const String& path) override;
};

//----------------------------------------------------------------------------//

class MeshCache final : public sq::ResourceCache<String, sq::Mesh>
{
    public:  MeshCache(); ~MeshCache() override;
    private: UniquePtr<sq::Mesh> create(const String& path) override;
};

//----------------------------------------------------------------------------//

class ArmatureCache final : public sq::ResourceCache<String, sq::Armature>
{
    public:  ArmatureCache(); ~ArmatureCache() override;
    private: UniquePtr<sq::Armature> create(const String& path) override;
};

//----------------------------------------------------------------------------//

class SoundCache final : public sq::ResourceCache<String, sq::SoundWave>
{
    public:  SoundCache(); ~SoundCache() override;
    private: UniquePtr<sq::SoundWave> create(const String& path) override;
};

//----------------------------------------------------------------------------//

class MaterialCache final : public sq::ResourceCache<String, sq::Material>
{
    public:  MaterialCache(TextureCache&); ~MaterialCache() override;
    private: UniquePtr<sq::Material> create(const String& path) override;
    private: TextureCache& mTextureCache;
};

//============================================================================//

class ResourceCaches final : sq::NonCopyable
{
public: //====================================================//

    ResourceCaches();
    ~ResourceCaches();

    //--------------------------------------------------------//

    TextureCache  textures;
    MeshCache     meshes;
    ArmatureCache armatures;
    SoundCache    sounds;
    MaterialCache materials;
};

//============================================================================//

} // namespace sqt
