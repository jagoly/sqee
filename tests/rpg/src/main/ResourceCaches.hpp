#pragma once

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

using TextureHandle  = sq::Handle<sq::Texture2D>;
using MeshHandle     = sq::Handle<sq::Mesh>;
using ArmatureHandle = sq::Handle<sq::Armature>;
using SoundHandle    = sq::Handle<sq::Sound>;
using MaterialHandle = sq::Handle<sq::Material>;

//============================================================================//

class TextureCache final : public sq::ResourceCache<std::string, sq::Texture2D>
{
    public:  TextureCache(); ~TextureCache() override;
    private: std::unique_ptr<sq::Texture2D> create(const std::string& path) override;
};

//----------------------------------------------------------------------------//

class MeshCache final : public sq::ResourceCache<std::string, sq::Mesh>
{
    public:  MeshCache(); ~MeshCache() override;
    private: std::unique_ptr<sq::Mesh> create(const std::string& path) override;
};

//----------------------------------------------------------------------------//

class ArmatureCache final : public sq::ResourceCache<std::string, sq::Armature>
{
    public:  ArmatureCache(); ~ArmatureCache() override;
    private: std::unique_ptr<sq::Armature> create(const std::string& path) override;
};

//----------------------------------------------------------------------------//

class SoundCache final : public sq::ResourceCache<std::string, sq::Sound>
{
    public:  SoundCache(); ~SoundCache() override;
    private: std::unique_ptr<sq::Sound> create(const std::string& path) override;
};

//----------------------------------------------------------------------------//

class MaterialCache final : public sq::ResourceCache<std::string, sq::Material>
{
    public:  MaterialCache(TextureCache&); ~MaterialCache() override;
    private: std::unique_ptr<sq::Material> create(const std::string& path) override;
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
