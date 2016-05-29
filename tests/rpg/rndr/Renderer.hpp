#pragma once

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/ecs/Entity.hpp>
#include <sqee/gl/Shaders.hpp>

#include "../components/Helpers.hpp"

namespace sqt {

struct CameraData {
    vector<const ModelComponent*> modelSimpleVec;
    vector<const ModelComponent*> modelSkellyVec;
    vector<const SpotLightComponent*> spotLightShadowVec;
    vector<const SpotLightComponent*> spotLightNoShadowVec;
    vector<const PointLightComponent*> pointLightShadowVec;
    vector<const PointLightComponent*> pointLightNoShadowVec;
    vector<const DecalComponent*> decalCompleteVec;
    vector<const DecalComponent*> decalPartialVec;
};

struct SkyLightData {
    array<vector<const ModelComponent*>, 4> modelSimpleVecArrA;
    array<vector<const ModelComponent*>, 4> modelSimplePunchVecArrA;
    array<vector<const ModelComponent*>, 4> modelSkellyVecArrA;
    array<vector<const ModelComponent*>, 4> modelSkellyPunchVecArrA;
    array<vector<const ModelComponent*>, 2> modelSimpleVecArrB;
    array<vector<const ModelComponent*>, 2> modelSimplePunchVecArrB;
    array<vector<const ModelComponent*>, 2> modelSkellyVecArrB;
    array<vector<const ModelComponent*>, 2> modelSkellyPunchVecArrB;
};

struct SpotLightData {
    SpotLightData(const SpotLightComponent* _light)
        : light(_light) {}

    vector<const ModelComponent*> modelSimpleVec;
    vector<const ModelComponent*> modelSimplePunchVec;
    vector<const ModelComponent*> modelSkellyVec;
    vector<const ModelComponent*> modelSkellyPunchVec;
    const SpotLightComponent* const light;
};

struct PointLightData {
    PointLightData(const PointLightComponent* _light, array<bool, 6> _visible)
        : light(_light), visibleFaceArr(_visible) {}

    array<vector<const ModelComponent*>, 6> modelSimpleVecArr;
    array<vector<const ModelComponent*>, 6> modelSimplePunchVecArr;
    array<vector<const ModelComponent*>, 6> modelSkellyVecArr;
    array<vector<const ModelComponent*>, 6> modelSkellyPunchVecArr;
    const PointLightComponent* const light;
    const array<bool, 6> visibleFaceArr;
};

struct ReflectData {
    ReflectData(const ReflectComponent* _reflect, const ModelComponent* _model, sq::Frustum _frus)
        : reflect(_reflect), model(_model), frus(_frus) {}

    vector<const ModelComponent*> modelSimpleVec;
    vector<const ModelComponent*> modelSkellyVec;
    vector<const SpotLightComponent*> spotLightShadowVec;
    vector<const SpotLightComponent*> spotLightNoShadowVec;
    vector<const PointLightComponent*> pointLightShadowVec;
    vector<const PointLightComponent*> pointLightNoShadowVec;
    vector<const DecalComponent*> decalDiffVec;
    const ReflectComponent* const reflect;
    const ModelComponent* const model;
    const sq::Frustum frus;
};

//struct EmitterData {
//    EmitterData(const wcoe::Emitter* _emitr) : emitr(*_emitr) {}
//    vector<const wcoe::PointLight*> pointLightVec;
//    vector<const wcoe::SpotLight*> spotLightVec;
//    const wcoe::Emitter& emitr;
//};


class Renderer final : sq::NonCopyable {
public:
    Renderer(sq::MessageBus& _messageBus,
             const sq::Settings& _settings,
             const sq::PreProcessor& _preprocs,
             const sq::Pipeline& _pipeline,
             const World& _world);

    ~Renderer();

    void configure();
    void render_scene();
    void draw_debug_bounds();

    CameraData cameraData;
    SkyLightData skyLightData;
    vector<SpotLightData> spotLightDataVec;
    vector<PointLightData> pointLightDataVec;
    vector<ReflectData> reflectDataVec;

    float tickPercent = 0.f;

    /*struct {
        sq::Texture2D* pshadA = nullptr;
        sq::Texture2D* pshadB = nullptr;
        sq::Texture2D* partDpSt = nullptr;
        sq::Texture2D* partMain = nullptr;
        sq::Texture2D* hdrPart = nullptr;
    } TX;

    struct {
        sq::FrameBuffer* pshadA = nullptr;
        sq::FrameBuffer* pshadB = nullptr;
        sq::FrameBuffer* defrPart = nullptr;
        sq::FrameBuffer* hdrPart = nullptr;
    } FB;*/

private:
    /*sq::Shader VS_part_soft_vertex {gl::VERTEX_SHADER};
    sq::Shader GS_part_soft_geometry {gl::GEOMETRY_SHADER};
    sq::Shader FS_part_soft_ambient {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_skylight {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_spot_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_spot_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_point_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_point_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_write {gl::FRAGMENT_SHADER};*/

    sq::Shader VS_fullscreen {gl::VERTEX_SHADER};
    sq::Shader VS_stencil_base {gl::VERTEX_SHADER};
    sq::Shader VS_stencil_refl {gl::VERTEX_SHADER};
    sq::Shader FS_passthrough {gl::FRAGMENT_SHADER};
    sq::Shader FS_fill_space {gl::FRAGMENT_SHADER};

    class Shadows; unique_ptr<Shadows> shadows;
    class Gbuffers; unique_ptr<Gbuffers> gbuffers;
    class Lighting; unique_ptr<Lighting> lighting;
    class Pretties; unique_ptr<Pretties> pretties;
    class Reflects; unique_ptr<Reflects> reflects;

    sq::MessageBus& messageBus;
    const sq::Settings& settings;
    const sq::PreProcessor& preprocs;
    const sq::Pipeline& pipeline;
    const World& world;

    struct Impl;
    friend struct Impl;
    unique_ptr<Impl> impl;
};

}
