#pragma once

#include <map>
#include <set>

#include <sqee/core/Types.hpp>

#include <sqee/misc/DopClasses.hpp>

#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Material.hpp>
#include <sqee/objects/Armature.hpp>

using namespace sq::coretypes;

namespace sqt::render::data {

//============================================================================//
//  Depth Passes Data                                                         //
//============================================================================//

struct DepthModelSimplePass
{
    struct Full // a model with no punch materials
    {
        const sq::Mesh& mesh;  // reference to the mesh
        const Mat4F matrix;    // model-view-proj matrix
        const bool mirror;     // sign of determinant
    };

    struct Part // a single sub-mesh of the model
    {
        const sq::Mesh& mesh; // reference to the mesh
        const Mat4F matrix;   // model-view-proj matrix
        const bool mirror;    // sign of determinant
        const uint index;     // model material index
    };

    // vector containing fully solid models
    std::vector<Full> solidFullVec;

    // vector containing solid sub-meshes
    std::vector<Part> solidPartVec;

    // map of mask textures to vectors of sub-meshes
    std::map<const sq::Texture2D*, std::vector<Part>> punchPartMap;
};

//----------------------------------------------------------------------------//

struct DepthModelSkellyPass
{
    struct Full // a model with no punch materials
    {
        const sq::FixedBuffer& ubo;   // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const bool mirror;            // sign of determinant
    };

    struct Part // a single sub-mesh of the model
    {
        const sq::FixedBuffer& ubo;   // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const bool mirror;            // sign of determinant
        const uint index;             // model material index
    };

    // vector containing fully solid models
    std::vector<Full> solidFullVec;

    // vector containing solid sub-meshes
    std::vector<Part> solidPartVec;

    // map of mask textures to vectors of sub-meshes
    std::map<const sq::Texture2D*, std::vector<Part>> punchPartMap;
};

//----------------------------------------------------------------------------//

struct DepthPasses
{
    DepthModelSimplePass modelSimplePass;
    DepthModelSkellyPass modelSkellyPass;
};


//============================================================================//
//  Gbuffer Passes Data                                                       //
//============================================================================//

struct GbufferModelSimplePass
{
    struct Base // shared base data for a model instance
    {
        const sq::Mesh& mesh; // reference to the mesh
        const Mat4F matrix;   // model-view-proj matrix
        const Mat3F normMat;  // inverse transpose model-view
        const bool mirror;    // sign of determinant
    };

    struct Part // a single sub-mesh of a model
    {
        const Base& base; // reference to shared data
        const uint index; // model material index
        auto operator->() const { return &base; }
    };

    // vector containing shared base data
    std::vector<Base> baseVec;

    // map of materials to vectors of sub-meshes
    std::map<const sq::Material*, std::vector<Part>> partMap;
};

//----------------------------------------------------------------------------//

struct GbufferModelSkellyPass
{
    struct Base // shared base data for a model instance
    {
        const sq::FixedBuffer& ubo;   // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const Mat3F normMat;          // inverse transpose model-view
        const bool mirror;            // sign of determinant
    };

    struct Part // a single sub-mesh of a model
    {
        const Base& base; // reference to shared data
        const uint index; // model material index
        auto operator->() const { return &base; }
    };

    // vector containing shared base data
    std::vector<Base> baseVec;

    // map of materials to vectors of sub-meshes
    std::map<const sq::Material*, std::vector<Part>> partMap;
};

//----------------------------------------------------------------------------//

struct GbufferDecalBasicPass
{
    struct Decal // a single decal instance
    {
        const float alpha;     // overlay blending factor
        const Mat4F matrix;    // model-view matrix
        const Mat4F invMatrix; // inverse model-view matrix
    };

    // map of materials to vectors of decals
    std::map<const sq::Material*, std::vector<Decal>> decalMap;
};

//----------------------------------------------------------------------------//

struct GbufferModelPasses
{
    GbufferModelSimplePass simplePass;
    GbufferModelSkellyPass skellyPass;
};

struct GbufferDecalPasses
{
    GbufferDecalBasicPass basicPass;
};

struct GbufferPasses
{
    GbufferModelPasses modelPasses;
    GbufferDecalPasses decalPasses;
};


//============================================================================//
//  Shadows Passes Data                                                       //
//============================================================================//

struct ShadowsPass // data for a single shadow map pass
{
    const sq::FrameBuffer& fbo; // reference to the shadow map fbo
    const Vec2U viewPort;       // size of the shadow map view port
    DepthPasses depthPasses;    // the actual depth pass data
};

//----------------------------------------------------------------------------//

struct ShadowsPasses
{
    std::vector<ShadowsPass> cascPassVec;
    std::vector<ShadowsPass> orthoPassVec;
    std::vector<ShadowsPass> pointPassVec;
    std::vector<ShadowsPass> spotPassVec;
};


//============================================================================//
//  LightBase Passes Data                                                     //
//============================================================================//

struct LightBaseSkyBoxPass // data for a single skybox pass
{
    const sq::TextureCube& tex; // reference to the skybox cube texture
    const Vec4F params;         // saturation, brightness, contrast, opacity
};

//----------------------------------------------------------------------------//

struct LightBaseAmbientPass // data for a single ambient pass
{
    const Vec3F colour; // ambient lighting colour
};

//----------------------------------------------------------------------------//

struct LightBasePasses
{
    std::unique_ptr<LightBaseSkyBoxPass> skyboxPass;
    std::unique_ptr<LightBaseAmbientPass> ambientPass;
};


//============================================================================//
//  LightAccum Passes Data                                                    //
//============================================================================//

struct LightAccumSkyLightPass // data for a single cascade light pass
{
    const sq::FixedBuffer& ubo;  // reference to light's uniform buffer
    const sq::TextureArray& tex; // reference to light's shadow texture
};

//----------------------------------------------------------------------------//

struct LightAccumStencilPass // data for a single non-cascade light pass
{
    const sq::FixedBuffer& ubo;   // reference to light's uniform buffer
    const sq::Texture& tex;       // reference to light's shadow texture
    const Mat4F matrix;           // model-view-projection matrix
};

//----------------------------------------------------------------------------//

struct LightAccumPasses
{
    std::unique_ptr<LightAccumSkyLightPass> skylightPass;
    std::vector<LightAccumStencilPass> orthoPassVec;
    std::vector<LightAccumStencilPass> pointPassVec;
    std::vector<LightAccumStencilPass> spotPassVec;
};


//============================================================================//
//  Volumetric Passes Data                                                    //
//============================================================================//

struct VolumetricSkyLightPass // data for a single cascade light pass
{
    const sq::FixedBuffer& ubo;  // reference to light's uniform buffer
    const sq::TextureArray& tex; // reference to light's shadow texture
};

//----------------------------------------------------------------------------//

struct VolumetricStencilPass // data for a single non-cascade light pass
{
    const sq::FixedBuffer& ubo;   // reference to light's uniform buffer
    const sq::Texture& tex;       // reference to light's shadow texture
    const Mat4F matrix;           // model-view-projection matrix
    const bool stencil;           // is the camera outside of the volume
};

//----------------------------------------------------------------------------//

struct VolumetricPasses
{
    std::unique_ptr<VolumetricSkyLightPass> skylightPass;
    std::vector<VolumetricStencilPass> orthoPassVec;
    std::vector<VolumetricStencilPass> pointPassVec;
    std::vector<VolumetricStencilPass> spotPassVec;
};


//============================================================================//

} // namespace sqt::render::data

//============================================================================//

namespace sqt { class RenderStuff; class WorldStuff; };

//============================================================================//

namespace sqt::render {

//============================================================================//

class PassesData : sq::NonCopyable
{
public:

    void prepare(const WorldStuff& wstuff, const RenderStuff& rstuff);

    data::DepthPasses depthData;
    data::GbufferPasses gbufferData;
    data::ShadowsPasses shadowsData;
    data::LightBasePasses lightBaseData;
    data::LightAccumPasses lightAccumData;
    data::VolumetricPasses volumetricData;

private:

    void prepare_depth_models(const RenderStuff& stuff);

    void prepare_gbuffer_models(const RenderStuff& stuff);
    void prepare_gbuffer_decals(const RenderStuff& stuff);

    void prepare_lighting_basic(const RenderStuff& stuff);
    void prepare_lighting_accum(const RenderStuff& stuff);

    void prepare_shadow_mapping(const RenderStuff& stuff, const WorldStuff& wstuff);

    sq::dop::Group mVisibleSet;
    sq::dop::Group mModelSimpleSet;
    sq::dop::Group mModelSkellySet;
    sq::dop::Group mLightOrthoSet;
    sq::dop::Group mLightPointSet;
    sq::dop::Group mLightSpotSet;
};

} // namespace sqt::render
