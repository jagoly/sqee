#pragma once

#include <unordered_map>

#include <sqee/dop/Classes.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

#include <sqee/sound/Sound.hpp>

#include "../resources/Manager.hpp"
#include "../helpers.hpp"

//============================================================================//

namespace sqt { namespace sys {

//============================================================================//

//struct CullSphere { Vec3F origin; float radius; };
//struct CullBox { Vec3F origin; float radius; Vec3F extents; QuatF basis; };
//struct CullPlane { Vec3F normal; float offset; };
//using CullFrustum = std::array<CullPlane, 4>;

//============================================================================//

struct EntityData
{
    string uniqueName = "";
    int32_t parent = -1;
    uint8_t nesting = 0u;

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
    Vec3F stretch = {1.f, 1.f, 1.f};

    vector<HandleMaterial> materials;
    HandleMesh mesh;

    bool enableRender = true;
    bool enableShadow = true;
    bool enableDecals = true;

    Mat4F matrix = Mat4F();
};

//========================================================//

struct SkeletonData
{
    HandleArmature armature;
};

//========================================================//

struct PointLightData
{
    Vec3F colour = {1.f, 1.f, 1.f};
    uint resolution = 0u;
};

//========================================================//

struct SpotLightData
{
    Vec3F colour = {1.f, 1.f, 1.f};
    float softness = 0.f;
    float angle = 0.125f;
    uint resolution = 0u;
};

//============================================================================//

struct SoundData
{
    sq::Sound sound;
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

struct WorldStuff : sq::NonCopyable
{
    int32_t entityCounter = -1;
    int32_t soundCounter = -1;

    //========================================================//

    struct {
        sq::dop::Table<EntityData>     entity;
        sq::dop::Table<AnimationData>  animation;
        sq::dop::Table<TransformData>  transform;
        sq::dop::Table<ModelData>      model;
        sq::dop::Table<SkeletonData>   skeleton;
        sq::dop::Table<PointLightData> pointLight;
        sq::dop::Table<SpotLightData>  spotLight;
    } tables;

    //========================================================//

    struct {
        sq::dop::Group modelSimple;
        sq::dop::Group modelSkelly;
        sq::dop::Group decalBasic;

        sq::dop::Group lightOrtho;
        sq::dop::Group lightPoint;
        sq::dop::Group lightSpot;
    } groups;

    //========================================================//

    std::unordered_map<string, int32_t> uniqueNames;

    //========================================================//

    struct {
        sq::dop::Table<std::vector<AnimTransformData>> transform;
        //dop::DataTable<std::vector<AnimPointLightData>> pointLight;
        //dop::DataTable<std::vector<AnimSpotLightData>> spotLight;
    } animation;

    //========================================================//

    struct {
        sq::dop::Table<sq::maths::Box>    mainBoxes;
        sq::dop::Table<sq::maths::Sphere> mainSpheres;

//        dop::DataTable<CullBox>    modelBoxes;
//        dop::DataTable<CullSphere> modelSpheres;

//        dop::DataTable<CullBox>    lightBoxes;
//        dop::DataTable<CullSphere> lightSpheres;

        sq::dop::Table<CullPointLightData> pointLights;
        sq::dop::Table<CullSpotLightData>  spotLights;
    } culling;

    //========================================================//

    struct {
        sq::dop::Table<SoundData> table;

        // 0: EFFECTS | 1: SPEECH | 2: MUSIC /////
        array<float, 3> groupVolumes {{1.f, 1.f, 1.f}};
    } sound;

    //========================================================//

private:

    WorldStuff(); ~WorldStuff();
    friend WorldStuff& static_WorldStuff();
};

//============================================================================//

WorldStuff& static_WorldStuff();

//============================================================================//

}} // namespace sqt::sys
