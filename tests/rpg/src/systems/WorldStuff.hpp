#pragma once

#include <sqee/misc/DopClasses.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/Textures.hpp>
#include <sqee/objects/Material.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Armature.hpp>
#include <sqee/objects/Sound.hpp>

#include "../main/ResourceCaches.hpp"

#include "../world/Camera.hpp"
#include "../world/Skybox.hpp"
#include "../world/Ambient.hpp"
#include "../world/Skylight.hpp"

//============================================================================//

namespace sqt::sys {

//============================================================================//

struct EntityData
{
    std::string uniqueName = "";
    int32_t parent = -1;

    sq::dop::Group children;
};

//============================================================================//

struct AnimationData
{
    std::vector<uint32_t> times;
    uint32_t index, progress;

    std::function<void(int32_t id)> callback_on_end;
};

//========================================================//

struct TransformData
{
    Mat4F localMatrix = Mat4F();
    Vec3F localPosition = Vec3F();
    QuatF localRotation = QuatF();
    float localScale = 1.f;

    Mat4F worldMatrix = Mat4F();
    Vec3F worldPosition = Vec3F();
    QuatF worldRotation = QuatF();
    float worldScale = 1.f;
};

//========================================================//

struct ModelData
{
    Vec3F stretch = { 1.f, 1.f, 1.f };

    std::vector<MaterialHandle> materials;
    MeshHandle mesh;

    bool enableRender = true;
    bool enableShadow = true;
    bool enableDecals = true;

    Mat4F matrix = Mat4F();
};

//========================================================//

struct SkeletonData
{
    ArmatureHandle armature;
};

//========================================================//

struct OrthoLightData
{
    Vec3F colour = { 1.f, 1.f, 1.f };
    Vec3F minimum = { -1.f, -1.f, -1.f };
    Vec3F maximum = { +1.f, +1.f, +1.f };
    float density = 0.f;
    uint resolution = 0u;
};

struct PointLightData
{
    Vec3F colour = { 1.f, 1.f, 1.f };
    uint resolution = 0u;
};

struct SpotLightData
{
    Vec3F colour = { 1.f, 1.f, 1.f };
    float softness = 0.f;
    float angle = 0.125f;
    uint resolution = 0u;
};

//============================================================================//

struct SoundData
{
    //sq::Sound sound;
    uint8_t group;
    int32_t entity;
    float volume;
};

//============================================================================//

struct AnimTransformData { Vec3F position; QuatF rotation; float scale; };

struct AnimModelData { Vec3F stretch; };

struct AnimPointLightData { Vec3F colour; };

struct AnimSpotLightData { Vec3F colour; float softness; float angle; };

//============================================================================//

struct CullPointLightData { sq::maths::Sphere sphere; sq::maths::Frustum frustums[6]; };

struct CullSpotLightData { sq::maths::Frustum frustum; };

//============================================================================//

} // namespace sqt::sys

namespace sqt {

class WorldStuff final : sq::NonCopyable
{
public: //====================================================//

    WorldStuff();

    ~WorldStuff();

    //--------------------------------------------------------//

    int32_t entityCounter = -1;
    int32_t soundCounter = -1;

    //--------------------------------------------------------//

    struct {
        sq::dop::Table<sys::EntityData>     entity;
        sq::dop::Table<sys::AnimationData>  animation;
        sq::dop::Table<sys::TransformData>  transform;
        sq::dop::Table<sys::ModelData>      model;
        sq::dop::Table<sys::SkeletonData>   skeleton;
        sq::dop::Table<sys::OrthoLightData> ortholight;
        sq::dop::Table<sys::PointLightData> pointlight;
        sq::dop::Table<sys::SpotLightData>  spotlight;
    } tables;

    //--------------------------------------------------------//

    struct {
        sq::dop::Group modelSimple;
        sq::dop::Group modelSkelly;
        sq::dop::Group decalBasic;
        sq::dop::Group lightOrtho;
        sq::dop::Group lightPoint;
        sq::dop::Group lightSpot;
    } groups;

    //--------------------------------------------------------//

    std::unordered_map<std::string, int32_t> uniqueNames;

    sq::dop::Group deletedEntities;

    //--------------------------------------------------------//

    struct {
        sq::dop::Table<sq::maths::Box>    mainBoxes;
        sq::dop::Table<sq::maths::Sphere> mainSpheres;

        sq::dop::Table<sys::CullPointLightData> pointLights;
        sq::dop::Table<sys::CullSpotLightData>  spotLights;
    } culling;

    //--------------------------------------------------------//

    struct {
        sq::dop::Table<std::vector<sys::AnimTransformData>> transform;
        //dop::DataTable<std::vector<AnimOrthoLightData>> orthoLight;
        //dop::DataTable<std::vector<AnimPointLightData>> pointLight;
        //dop::DataTable<std::vector<AnimSpotLightData>> spotLight;
    } animation;

    //--------------------------------------------------------//

    sq::dop::Table<sys::SoundData> soundTable;
    std::array<float, 3> soundGroupVolumes {{1.f, 1.f, 1.f}};

    //--------------------------------------------------------//

    sq::dop::Group nestingRootGroup;
    std::array<sq::dop::Table<int32_t>, 8> nestingTables;

    //--------------------------------------------------------//

    world::Camera camera;

    std::unique_ptr<world::Skybox> skybox;
    std::unique_ptr<world::Ambient> ambient;
    std::unique_ptr<world::Skylight> skylight;
};

//============================================================================//

} // namespace sqt
