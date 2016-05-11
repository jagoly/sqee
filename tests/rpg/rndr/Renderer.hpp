#pragma once

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/MessageBus.hpp>
#include <sqee/ecs/Entity.hpp>
#include <sqee/gl/Shaders.hpp>


namespace sqt {

namespace wcoe { class World; }

class TransformComponent;
class ModelComponent;
class SpotLightComponent;
class PointLightComponent;
class ReflectComponent;
class DecalComponent;

namespace rndr {

class Shadows;
class Gbuffers;
class Lighting;
class Pretties;
class Reflects;

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
    SpotLightData(const SpotLightComponent* _light) : light(_light) {}
    vector<const ModelComponent*> modelSimpleVec;
    vector<const ModelComponent*> modelSimplePunchVec;
    vector<const ModelComponent*> modelSkellyVec;
    vector<const ModelComponent*> modelSkellyPunchVec;
    const SpotLightComponent* const light;
};

struct PointLightData {
    PointLightData(const PointLightComponent* _light, array<bool, 6> _visible) : light(_light), visibleFaceArr(_visible) {}
    array<vector<const ModelComponent*>, 6> modelSimpleVecArr;
    array<vector<const ModelComponent*>, 6> modelSimplePunchVecArr;
    array<vector<const ModelComponent*>, 6> modelSkellyVecArr;
    array<vector<const ModelComponent*>, 6> modelSkellyPunchVecArr;
    const PointLightComponent* const light;
    const array<bool, 6> visibleFaceArr;
};

struct ReflectData {
    ReflectData(const ReflectComponent* _reflect, sq::Frustum _frus) : reflect(_reflect), frus(_frus) {}
    vector<const ModelComponent*> modelSimpleVec;
    vector<const ModelComponent*> modelSkellyVec;
    vector<const SpotLightComponent*> spotLightShadowVec;
    vector<const SpotLightComponent*> spotLightNoShadowVec;
    vector<const PointLightComponent*> pointLightShadowVec;
    vector<const PointLightComponent*> pointLightNoShadowVec;
    vector<const DecalComponent*> decalDiffVec;
    const ReflectComponent* const reflect;
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
    Renderer(sq::MessageBus& _messageBus, const sq::Settings& _settings,
             const sq::PreProcessor& _preprocs, const sq::Pipeline& _pipeline,
             wcoe::World& _world);

    ~Renderer();

    void update_settings();

    void prepare_render_stuff();


    // E //////
    void render_particles();

    CameraData cameraData;
    SkyLightData skyLightData;
    vector<SpotLightData> spotLightDataVec;
    vector<PointLightData> pointLightDataVec;
    vector<ReflectData> reflectDataVec;

    const sq::Camera* camera = nullptr;

    struct {
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
    } FB;

    unique_ptr<Shadows> shadows;
    unique_ptr<Gbuffers> gbuffers;
    unique_ptr<Lighting> lighting;
    unique_ptr<Pretties> pretties;
    unique_ptr<Reflects> reflects;

    void draw_debug_bounds();

private:
    friend class Shadows;
    friend class Gbuffers;
    friend class Lighting;
    friend class Pretties;
    friend class Reflects;

    sq::Shader VS_part_soft_vertex {gl::VERTEX_SHADER};
    sq::Shader GS_part_soft_geometry {gl::GEOMETRY_SHADER};
    sq::Shader FS_part_soft_ambient {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_skylight {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_spot_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_spot_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_point_none {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_point_shad {gl::FRAGMENT_SHADER};
    sq::Shader FS_part_soft_write {gl::FRAGMENT_SHADER};

    sq::Shader VS_fullscreen {gl::VERTEX_SHADER};
    sq::Shader VS_stencil_base {gl::VERTEX_SHADER};
    sq::Shader VS_stencil_refl {gl::VERTEX_SHADER};
    sq::Shader FS_passthrough {gl::FRAGMENT_SHADER};
    sq::Shader FS_fill_space {gl::FRAGMENT_SHADER};

    //const wcoe::Emitter* crntEmitr = nullptr;

    sq::MessageBus& messageBus;

    const sq::Settings& settings;
    const sq::PreProcessor& preprocs;
    const sq::Pipeline& pipeline;
    wcoe::World& world;

    GLuint partVAO = 0u, partVBO = 0u, partIBO = 0u;

    struct Implementation;
    friend struct Implementation;
    unique_ptr<Implementation> impl;
};

}}
