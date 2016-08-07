#pragma once

#include <map>
#include <set>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/ecs/Entity.hpp>

#include "../resources/Material.hpp"

namespace sqt {

class SceneData;

namespace render { namespace data {


// Depth Passes Data //////////

struct DepthModelSimplePass {

    struct Full { /// a model with no punch materials
        const sq::Mesh& mesh; // reference to the mesh
        const Mat4F matrix;   // model-view-proj matrix
        const bool mirror;    // sign of determinant
    };

    struct Part { /// a single sub-mesh of the model
        const sq::Mesh& mesh; // reference to the mesh
        const Mat4F matrix;   // model-view-proj matrix
        const bool mirror;    // sign of determinant
        const uint index;     // model material index
    };

    // vector containing fully solid models
    vector<Full> solidFullVec;

    // vector containing solid sub-meshes
    vector<Part> solidPartVec;

    // map of mask textures to vectors of sub-meshes
    std::map<const sq::Texture2D*, vector<Part>> punchPartMap;

};

struct DepthModelSkellyPass {

    struct Full { /// a model with no punch materials
        const sq::UniformBuffer& ubo; // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const bool mirror;            // sign of determinant
    };

    struct Part { /// a single sub-mesh of the model
        const sq::UniformBuffer& ubo; // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const bool mirror;            // sign of determinant
        const uint index;             // model material index
    };

    // vector containing fully solid models
    vector<Full> solidFullVec;

    // vector containing solid sub-meshes
    vector<Part> solidPartVec;

    // map of mask textures to vectors of sub-meshes
    std::map<const sq::Texture2D*, vector<Part>> punchPartMap;

};

struct DepthPasses {
    DepthModelSimplePass modelSimplePass;
    DepthModelSkellyPass modelSkellyPass;
};



// Gbuffer Passes Data //////////

struct GbufferModelSimplePass {

    struct Base { /// shared base data for a model instance
        const sq::Mesh& mesh; // reference to the mesh
        const Mat4F matrix;   // model-view-proj matrix
        const Mat3F normMat;  // inverse transpose model-view
        const bool mirror;    // sign of determinant
    };

    struct Part { /// a single sub-mesh of a model
        const Base& base; // reference to shared data
        const uint index; // model material index
        auto operator->() const { return &base; };
    };

    // vector containing shared base data
    vector<Base> baseVec;

    // map of materials to vectors of sub-meshes
    std::map<const Material*, vector<Part>> partMap;

};

struct GbufferModelSkellyPass {

    struct Base { /// shared base data for a model instance
        const sq::UniformBuffer& ubo; // reference to bones buffer
        const sq::Mesh& mesh;         // reference to the mesh
        const Mat4F matrix;           // model-view-proj matrix
        const Mat3F normMat;          // inverse transpose model-view
        const bool mirror;            // sign of determinant
    };

    struct Part { /// a single sub-mesh of a model
        const Base& base; // reference to shared data
        const uint index; // model material index
        auto operator->() const { return &base; };
    };

    // vector containing shared base data
    vector<Base> baseVec;

    // map of materials to vectors of sub-meshes
    std::map<const Material*, vector<Part>> partMap;

};

struct GbufferDecalBasicPass {

    struct Decal { /// a single decal instance
        const float alpha;     // overlay blending factor
        const Mat4F matrix;    // model-view matrix
        const Mat4F invMatrix; // inverse model-view matrix
    };

    // map of materials to vectors of decals
    std::map<const Material*, vector<Decal>> decalMap;
};

struct GbufferModelPasses {
    GbufferModelSimplePass simplePass;
    GbufferModelSkellyPass skellyPass;
};

struct GbufferDecalPasses {
    GbufferDecalBasicPass basicPass;
};

struct GbufferPasses {
    GbufferModelPasses modelPasses;
    GbufferDecalPasses decalPasses;
};



// Shadows Passes Data //////////

struct ShadowsPass { /// data for a single shadow map pass
    const sq::FrameBuffer& fbo; // reference to the shadow map fbo
    const Vec2U viewPort;       // size of the shadow map view port
    DepthPasses depthPasses;    // the actual depth pass data
};

struct ShadowsPasses {
    vector<ShadowsPass> cascPassVec;
    vector<ShadowsPass> orthoPassVec;
    vector<ShadowsPass> pointPassVec;
    vector<ShadowsPass> spotPassVec;
};



// LightBase Passes Data //////////

struct LightBaseSkyBoxPass { /// data for a single skybox pass
    const sq::TextureCube& tex; // reference to the skybox cube texture
    const Vec4F params;         // saturation, brightness, contrast, opacity
};

struct LightBaseAmbientPass { /// data for a single ambient pass
    const Vec3F colour; // ambient lighting colour
};

struct LightBasePasses {
    unique_ptr<LightBaseSkyBoxPass> skyboxPass;
    unique_ptr<LightBaseAmbientPass> ambientPass;
};



// LightAccum Passes Data //////////

struct LightAccumSkyLightPass { /// data for a single cascade light pass
    const sq::UniformBuffer& ubo;  // reference to light's uniform buffer
    const sq::Texture2DArray& tex; // referene to light's shadow texture
};

struct LightAccumStencilPass { /// data for a single non-cascade light pass
    const sq::UniformBuffer& ubo; // reference to light's uniform buffer
    const sq::Texture& tex;       // referene to light's shadow texture
    const Mat4F matrix;           // model-view-projection matrix
};

struct LightAccumPasses {
    unique_ptr<LightAccumSkyLightPass> skylightPass;
    vector<LightAccumStencilPass> orthoPassVec;
    vector<LightAccumStencilPass> pointPassVec;
    vector<LightAccumStencilPass> spotPassVec;
};



// Volumetric Passes Data //////////

struct VolumetricSkyLightPass { /// data for a single cascade light pass
    const sq::UniformBuffer& ubo;  // reference to light's uniform buffer
    const sq::Texture2DArray& tex; // referene to light's shadow texture
};

struct VolumetricStencilPass { /// data for a single non-cascade light pass
    const sq::UniformBuffer& ubo; // reference to light's uniform buffer
    const sq::Texture& tex;       // referene to light's shadow texture
    const Mat4F matrix;           // model-view-projection matrix
    const bool stencil;           // is the camera outside of the volume
};

struct VolumetricPasses {
    unique_ptr<VolumetricSkyLightPass> skylightPass;
    vector<VolumetricStencilPass> orthoPassVec;
    vector<VolumetricStencilPass> pointPassVec;
    vector<VolumetricStencilPass> spotPassVec;
};


} // namespace data


class ObjectsData; // Forward Declaration


class PassesData {
public:
    void prepare(const ObjectsData& _objects);

    data::DepthPasses depthData;
    data::GbufferPasses gbufferData;
    data::ShadowsPasses shadowsData;
    data::LightBasePasses lightBaseData;
    data::LightAccumPasses lightAccumData;
    data::VolumetricPasses volumetricData;

private:
    void prepare_depth_models(const ObjectsData& _objects);

    void prepare_gbuffer_models(const ObjectsData& _objects);
    void prepare_gbuffer_decals(const ObjectsData& _objects);

    void prepare_lighting_basic(const ObjectsData& _objects);
    void prepare_lighting_accum(const ObjectsData& _objects);

    void prepare_shadow_mapping(const ObjectsData& _objects);

    std::set<const sq::Entity*> visibleOrthoSet, visibleSpotSet;
    std::map<const sq::Entity*, array<bool, 6>> visiblePointMap;

};

}} // namespace sqt::render
