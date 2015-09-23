#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {

namespace wcoe { class World; class Cell;
                 class ModelSimple; class ModelSkelly;
                 class PointLight; class SpotLight;
                 class Reflector; class Emitter;
                 class Liquid; class Decal; }

namespace rndr {

using ModelSimpleList = list<weak_ptr<wcoe::ModelSimple>>;
using ModelSkellyList = list<weak_ptr<wcoe::ModelSkelly>>;
using PointLightList  = list<weak_ptr<wcoe::PointLight>>;
using SpotLightList   = list<weak_ptr<wcoe::SpotLight>>;
using ReflectorList   = list<weak_ptr<wcoe::Reflector>>;
using EmitterList     = list<weak_ptr<wcoe::Emitter>>;
using LiquidList      = list<weak_ptr<wcoe::Liquid>>;
using DecalList       = list<weak_ptr<wcoe::Decal>>;

class IrrVolume : NonCopyable {
public:
    IrrVolume();

    struct Probe {
        fvec3 position;
        array<fmat4, 6> matArr;
        unique_ptr<sq::UniformBuffer> ubo;
    };

    //array<Probe, 15*9*10> tree;
    array<Probe, 7*3*3> tree;

    unique_ptr<sq::Texture2DArray> texDiff;
    unique_ptr<sq::Texture2DArray> texSurf;
    unique_ptr<sq::Texture2DArray> texDpSt;
    unique_ptr<sq::Texture2DArray> texHdr;
    unique_ptr<sq::Texture2D> texEnvSHM;
    unique_ptr<sq::Texture2D> texEnvSDV;
};

class Graph : NonCopyable {
public:
    Graph(const sq::Settings& _settings, sq::Camera* _camera);

    // A //////
    void update();
    void reload_lists();
    void update_settings();
    void refresh_IrrVolTree();
    void render_envspheres();

    // B //////
    void render_shadows_sky_main();
    void render_shadows_sky_box();
    void render_shadows_spot();
    void render_shadows_point();

    // C //////
    void render_msimples_base(bool _decals);
    void render_msimples_refl(bool _decals);
    void render_mskellys_base(bool _decals);
    void render_mskellys_refl(bool _decals);
    void render_reflects_base(bool _decals);
    void render_reflects_refl(bool _decals);
    void render_decals_base();
    void render_decals_refl();

    // D //////
    void render_skybox_base();
    void render_skybox_refl();
    void render_ambient_base();
    void render_ambient_refl();
    void render_skylight_base();
    void render_skylight_refl();
    void render_spotlights_base();
    void render_spotlights_refl();
    void render_pointlights_base();
    void render_pointlights_refl();

    // E //////
    void render_reflections();
    void render_particles();

    wcoe::World* world = nullptr;
    ModelSimpleList modelSimpleList;
    ModelSkellyList modelSkellyList;
    PointLightList  pointLightList;
    SpotLightList   spotLightList;
    ReflectorList   reflectorList;
    EmitterList     emitterList;
    LiquidList      liquidList;
    DecalList       decalList;

    struct {
        sq::Shader* gnrc_screen = nullptr;
        sq::Shader* gbuf_base_stencil = nullptr;
        sq::Shader* gbuf_base_simple = nullptr;
        sq::Shader* gbuf_base_skelly = nullptr;
        sq::Shader* gbuf_base_decal = nullptr;
        sq::Shader* gbuf_refl_stencil = nullptr;
        sq::Shader* gbuf_refl_simple = nullptr;
        sq::Shader* gbuf_refl_skelly = nullptr;
        sq::Shader* gbuf_refl_decal = nullptr;
        sq::Shader* shad_simple = nullptr;
        sq::Shader* shad_skelly = nullptr;
        sq::Shader* irrd_gbuf_simple = nullptr;
        sq::Shader* irrd_defr_skybox = nullptr;
        sq::Shader* defr_reflector = nullptr;
        sq::Shader* defr_base_skybox = nullptr;
        sq::Shader* defr_refl_skybox = nullptr;
        sq::Shader* part_soft_vertex = nullptr;
        sq::Shader* dbug_envsphere = nullptr;
    } VS;

    struct {
        sq::Shader* part_soft_geometry = nullptr;
    } GS;

    struct {
        sq::Shader* gnrc_passthru = nullptr;
        sq::Shader* gnrc_lumalpha = nullptr;
        sq::Shader* gbuf_base_model = nullptr;
        sq::Shader* gbuf_base_decal = nullptr;
        sq::Shader* gbuf_refl_model = nullptr;
        sq::Shader* gbuf_refl_decal = nullptr;
        sq::Shader* shad_punch = nullptr;
        sq::Shader* irrd_gbuf_model = nullptr;
        sq::Shader* irrd_defr_skybox = nullptr;
        sq::Shader* irrd_defr_skylight = nullptr;
        sq::Shader* irrd_defr_spotlight = nullptr;
        sq::Shader* irrd_defr_pointlight = nullptr;
        sq::Shader* irrd_harmonics_shm = nullptr;
        sq::Shader* irrd_harmonics_sdv = nullptr;
        sq::Shader* defr_skybox = nullptr;
        sq::Shader* defr_reflector = nullptr;
        sq::Shader* defr_base_ambient = nullptr;
        sq::Shader* defr_base_skylight = nullptr;
        sq::Shader* defr_base_spot_none = nullptr;
        sq::Shader* defr_base_spot_shad = nullptr;
        sq::Shader* defr_base_spot_spec = nullptr;
        sq::Shader* defr_base_spot_both = nullptr;
        sq::Shader* defr_base_point_none = nullptr;
        sq::Shader* defr_base_point_shad = nullptr;
        sq::Shader* defr_base_point_spec = nullptr;
        sq::Shader* defr_base_point_both = nullptr;
        sq::Shader* defr_refl_ambient = nullptr;
        sq::Shader* defr_refl_skylight = nullptr;
        sq::Shader* defr_refl_spot_none = nullptr;
        sq::Shader* defr_refl_spot_shad = nullptr;
        sq::Shader* defr_refl_point_none = nullptr;
        sq::Shader* defr_refl_point_shad = nullptr;
        sq::Shader* part_soft_ambient = nullptr;
        sq::Shader* part_soft_skylight = nullptr;
        sq::Shader* part_soft_spot_none = nullptr;
        sq::Shader* part_soft_spot_shad = nullptr;
        sq::Shader* part_soft_point_none = nullptr;
        sq::Shader* part_soft_point_shad = nullptr;
        sq::Shader* part_soft_write = nullptr;
        sq::Shader* dbug_envsphere_shm = nullptr;
        sq::Shader* dbug_envsphere_sdv = nullptr;
    } FS;

    struct {
        sq::TextureMut2D* ssaoA = nullptr;
        sq::TextureMut2D* ssaoB = nullptr;
        sq::TextureMut2D* pshadA = nullptr;
        sq::TextureMut2D* pshadB = nullptr;
        sq::TextureMut2D* bloomA = nullptr;
        sq::TextureMut2D* bloomB = nullptr;
        sq::TextureMut2D* baseDpSt = nullptr;
        sq::TextureMut2D* baseDiff = nullptr;
        sq::TextureMut2D* baseSurf = nullptr;
        sq::TextureMut2D* baseNorm = nullptr;
        sq::TextureMut2D* baseSpec = nullptr;
        sq::TextureMut2D* reflDpSt = nullptr;
        sq::TextureMut2D* reflDiff = nullptr;
        sq::TextureMut2D* reflSurf = nullptr;
        sq::TextureMut2D* partDpSt = nullptr;
        sq::TextureMut2D* partMain = nullptr;
        sq::TextureMut2D* depHalf = nullptr;
        sq::TextureMut2D* depQter = nullptr;
        sq::TextureMut2D* hdrBase = nullptr;
        sq::TextureMut2D* hdrRefl = nullptr;
        sq::TextureMut2D* hdrPart = nullptr;
        sq::TextureMut2D* simple = nullptr;
    } TX;

    struct {
        sq::FrameBuffer* ssaoA = nullptr;
        sq::FrameBuffer* ssaoB = nullptr;
        sq::FrameBuffer* pshadA = nullptr;
        sq::FrameBuffer* pshadB = nullptr;
        sq::FrameBuffer* bloomA = nullptr;
        sq::FrameBuffer* bloomB = nullptr;
        sq::FrameBuffer* defrBase = nullptr;
        sq::FrameBuffer* defrRefl = nullptr;
        sq::FrameBuffer* defrPart = nullptr;
        sq::FrameBuffer* hdrBase = nullptr;
        sq::FrameBuffer* hdrPart = nullptr;
        sq::FrameBuffer* hdrRefl = nullptr;
        sq::FrameBuffer* simple = nullptr;
    } FB;

    struct {
        float viewdist;
        bool shadlarge;
        bool shadfilter;
        bool irrd;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        fvec2 fPixSize;
        fvec2 hPixSize;
        fvec2 qPixSize;
    } INFO;

    const sq::Settings& settings;
    const sq::Camera* const camera;
    sq::Pipeline* pipeline = nullptr;

    IrrVolume irrVolume;

private:
    const wcoe::Reflector* crntRflct = nullptr;
    const wcoe::Emitter* crntEmitr = nullptr;

    GLuint partVAO = 0u, partVBO = 0u, partIBO = 0u;
};

}}
