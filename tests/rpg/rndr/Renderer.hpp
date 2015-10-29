#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/Shaders.hpp>

namespace sqt {

namespace wcoe { class World;
                 class SkyLight;
                 class SpotLight;
                 class PointLight;
                 class ModelSimple;
                 class ModelSkelly;
                 class Reflector;
                 class Emitter;
                 class Decal; }

namespace rndr {

class Shadows;
class Gbuffers;
class Lighting;
class Pretties;
class Reflects;

struct CameraData {
    vector<const wcoe::Reflector*> reflectorVec;
    vector<const wcoe::ModelSimple*> modelSimpleVec;
    vector<const wcoe::ModelSkelly*> modelSkellyVec;
    vector<const wcoe::PointLight*> pointLightVec;
    vector<const wcoe::SpotLight*> spotLightVec;
    vector<const wcoe::Decal*> decalVec;
};

struct SkyLightData {
    array<vector<const wcoe::Reflector*>, 4> reflectorVecArrA;
    array<vector<const wcoe::Reflector*>, 4> reflectorPTVecArrA;
    array<vector<const wcoe::ModelSimple*>, 4> modelSimpleVecArrA;
    array<vector<const wcoe::ModelSimple*>, 4> modelSimplePTVecArrA;
    array<vector<const wcoe::ModelSkelly*>, 4> modelSkellyVecArrA;
    array<vector<const wcoe::ModelSkelly*>, 4> modelSkellyPTVecArrA;
    array<vector<const wcoe::Reflector*>, 2> reflectorVecArrB;
    array<vector<const wcoe::Reflector*>, 2> reflectorPTVecArrB;
    array<vector<const wcoe::ModelSimple*>, 2> modelSimpleVecArrB;
    array<vector<const wcoe::ModelSimple*>, 2> modelSimplePTVecArrB;
    array<vector<const wcoe::ModelSkelly*>, 2> modelSkellyVecArrB;
    array<vector<const wcoe::ModelSkelly*>, 2> modelSkellyPTVecArrB;
};

struct SpotLightData {
    SpotLightData(const wcoe::SpotLight* _light) : light(*_light) {}
    vector<const wcoe::Reflector*> reflectorVec;
    vector<const wcoe::Reflector*> reflectorPTVec;
    vector<const wcoe::ModelSimple*> modelSimpleVec;
    vector<const wcoe::ModelSimple*> modelSimplePTVec;
    vector<const wcoe::ModelSkelly*> modelSkellyVec;
    vector<const wcoe::ModelSkelly*> modelSkellyPTVec;
    const wcoe::SpotLight& light;
};

struct PointLightData {
    PointLightData(const wcoe::PointLight* _light) : light(*_light) {}
    array<vector<const wcoe::Reflector*>, 6> reflectorVecArr;
    array<vector<const wcoe::Reflector*>, 6> reflectorPTVecArr;
    array<vector<const wcoe::ModelSimple*>, 6> modelSimpleVecArr;
    array<vector<const wcoe::ModelSimple*>, 6> modelSimplePTVecArr;
    array<vector<const wcoe::ModelSkelly*>, 6> modelSkellyVecArr;
    array<vector<const wcoe::ModelSkelly*>, 6> modelSkellyPTVecArr;
    array<bool, 6> cullShadowFaceArr {{1,1,1,1,1,1}};
    const wcoe::PointLight& light;
};

struct ReflectorData {
    ReflectorData(const wcoe::Reflector* _rflct) : rflct(*_rflct) {}
    vector<const wcoe::Reflector*> reflectorVec;
    vector<const wcoe::ModelSimple*> modelSimpleVec;
    vector<const wcoe::ModelSkelly*> modelSkellyVec;
    vector<const wcoe::PointLight*> pointLightVec;
    vector<const wcoe::SpotLight*> spotLightVec;
    vector<const wcoe::Decal*> decalVec;
    const wcoe::Reflector& rflct;
};

struct EmitterData {
    EmitterData(const wcoe::Emitter* _emitr) : emitr(*_emitr) {}
    vector<const wcoe::PointLight*> pointLightVec;
    vector<const wcoe::SpotLight*> spotLightVec;
    const wcoe::Emitter& emitr;
};


class Renderer final : sq::NonCopyable {
public:
    Renderer(const sq::Settings& _settings, const sq::PreProcessor& _preprocs,
             const sq::Pipeline& _pipeline, const wcoe::World& _world);

    void reload_lists();
    void cull_and_sort();
    void update_settings();

    // E //////
    void render_particles();

    vector<wcoe::SpotLight*> spotLightVec;
    vector<wcoe::PointLight*> pointLightVec;
    vector<wcoe::ModelSimple*> modelSimpleVec;
    vector<wcoe::ModelSkelly*> modelSkellyVec;
    vector<wcoe::Reflector*> reflectorVec;
    vector<wcoe::Emitter*> emitterVec;
    vector<wcoe::Decal*> decalVec;

    CameraData cameraData;
    SkyLightData skyLightData;
    vector<SpotLightData> spotLightDataVec;
    vector<PointLightData> pointLightDataVec;
    vector<ReflectorData> reflectorDataVec;
    vector<EmitterData> emitterDataVec;

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

    const wcoe::Reflector* crntRflct = nullptr;
    const wcoe::Emitter* crntEmitr = nullptr;

    const sq::Settings& settings;
    const sq::PreProcessor& preprocs;
    const sq::Pipeline& pipeline;
    const wcoe::World& world;

    GLuint partVAO = 0u, partVBO = 0u, partIBO = 0u;
};

}}
