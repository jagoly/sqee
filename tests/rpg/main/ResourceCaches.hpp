#pragma once

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

using TextureHandle  = sq::Handle<sq::Texture2D>;
using MeshHandle     = sq::Handle<sq::Mesh>;
using ArmatureHandle = sq::Handle<sq::Armature>;
using SoundHandle    = sq::Handle<sq::SoundWave>;
using MaterialHandle = sq::Handle<sq::Material>;

//============================================================================//

class TextureCache final : public sq::ResourceCache<sq::Texture2D>
{
    public:  TextureCache(); ~TextureCache();
    private: unique_ptr<sq::Texture2D> create(const string& path) override;
};

//----------------------------------------------------------------------------//

class MeshCache final : public sq::ResourceCache<sq::Mesh>
{
    public:  MeshCache(); ~MeshCache();
    private: unique_ptr<sq::Mesh> create(const string& path) override;
};

//----------------------------------------------------------------------------//

class ArmatureCache final : public sq::ResourceCache<sq::Armature>
{
    public:  ArmatureCache(); ~ArmatureCache();
    private: unique_ptr<sq::Armature> create(const string& path) override;
};

//----------------------------------------------------------------------------//

class SoundCache final : public sq::ResourceCache<sq::SoundWave>
{
    public:  SoundCache(); ~SoundCache();
    private: unique_ptr<sq::SoundWave> create(const string& path) override;
};

//----------------------------------------------------------------------------//

class MaterialCache final : public sq::ResourceCache<sq::Material>
{
    public:  MaterialCache(TextureCache&); ~MaterialCache();
    private: unique_ptr<sq::Material> create(const string& path) override;
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
