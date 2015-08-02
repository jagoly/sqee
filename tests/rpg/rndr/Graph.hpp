#pragma once
#include <sqee/forward.hpp>

#include <list>

namespace sqt {

namespace wcoe { class World; class Cell;
                 class ModelStatic; class ModelSkelly;
                 class PointLight; class SpotLight;
                 class Reflector; class Emitter;
                 class Liquid; class Decal; }

namespace rndr {

using ModelStaticList = std::list<weak_ptr<wcoe::ModelStatic>>;
using ModelSkellyList = std::list<weak_ptr<wcoe::ModelSkelly>>;
using PointLightList  = std::list<weak_ptr<wcoe::PointLight>>;
using SpotLightList   = std::list<weak_ptr<wcoe::SpotLight>>;
using ReflectorList   = std::list<weak_ptr<wcoe::Reflector>>;
using EmitterList     = std::list<weak_ptr<wcoe::Emitter>>;
using LiquidList      = std::list<weak_ptr<wcoe::Liquid>>;
using DecalList       = std::list<weak_ptr<wcoe::Decal>>;

class Graph : NonCopyable {
public:
    Graph(sq::Camera* _camera, sq::SettingsMaps* _settings); // A

    void update();          // A
    void reload_lists();    // A
    void update_settings(); // A

    void render_shadows_sky_A(); // B
    void render_shadows_sky_B(); // B
    void render_shadows_spot();  // B
    void render_shadows_point(); // B
    void render_mstatics_base(bool _decals); // C
    void render_mstatics_refl(bool _decals); // C
    void render_mskellys_base(bool _decals); // C
    void render_mskellys_refl(bool _decals); // C
    void render_reflects_base(bool _decals); // C
    void render_reflects_refl(bool _decals); // C
    void render_decals_base();   // C
    void render_decals_refl();   // C

    void render_skybox_base();      // D
    void render_skybox_refl();      // D
    void render_ambient_base();     // D
    void render_ambient_refl();     // D
    void render_skylight_base();    // D
    void render_skylight_refl();    // D
    void render_spotlights_base();  // D
    void render_spotlights_refl();  // D
    void render_pointlights_base(); // D
    void render_pointlights_refl(); // D
    void render_reflections();      // E
    void render_particles();        // E

    wcoe::World* world = nullptr;
    ModelStaticList modelStaticList;
    ModelSkellyList modelSkellyList;
    PointLightList  pointLightList;
    SpotLightList   spotLightList;
    ReflectorList   reflectorList;
    EmitterList     emitterList;
    LiquidList      liquidList;
    DecalList       decalList;

    struct {
        sq::Shader* gnrc_screen = nullptr;
        sq::Shader* gbuf_stencil_base = nullptr;
        sq::Shader* gbuf_stencil_refl = nullptr;
        sq::Shader* gbuf_statics_base = nullptr;
        sq::Shader* gbuf_statics_refl = nullptr;
        sq::Shader* gbuf_skellys_base = nullptr;
        sq::Shader* gbuf_skellys_refl = nullptr;
        sq::Shader* gbuf_decals_base = nullptr;
        sq::Shader* gbuf_decals_refl = nullptr;
        sq::Shader* shad_static = nullptr;
        sq::Shader* shad_skelly = nullptr;
        sq::Shader* defr_skybox_base = nullptr;
        sq::Shader* defr_skybox_refl = nullptr;
        sq::Shader* defr_reflectors = nullptr;
        sq::Shader* part_vertex_soft = nullptr;
        sq::Shader* part_geometry_soft = nullptr;
    } VS;

    struct {
        sq::Shader* gnrc_fillwith = nullptr;
        sq::Shader* gnrc_passthru = nullptr;
        sq::Shader* gnrc_lumalpha = nullptr;
        sq::Shader* gbuf_models_base = nullptr;
        sq::Shader* gbuf_models_refl = nullptr;
        sq::Shader* gbuf_decals_base = nullptr;
        sq::Shader* gbuf_decals_refl = nullptr;
        sq::Shader* shad_punch = nullptr;
        sq::Shader* defr_skybox = nullptr;
        sq::Shader* defr_ambient_base = nullptr;
        sq::Shader* defr_ambient_refl = nullptr;
        sq::Shader* defr_skylight_base = nullptr;
        sq::Shader* defr_skylight_refl = nullptr;
        sq::Shader* defr_spot_none_base = nullptr;
        sq::Shader* defr_spot_none_refl = nullptr;
        sq::Shader* defr_spot_shad_base = nullptr;
        sq::Shader* defr_spot_shad_refl = nullptr;
        sq::Shader* defr_spot_spec_base = nullptr;
        sq::Shader* defr_spot_both_base = nullptr;
        sq::Shader* defr_point_none_base = nullptr;
        sq::Shader* defr_point_none_refl = nullptr;
        sq::Shader* defr_point_shad_base = nullptr;
        sq::Shader* defr_point_shad_refl = nullptr;
        sq::Shader* defr_point_spec_base = nullptr;
        sq::Shader* defr_point_both_base = nullptr;
        sq::Shader* defr_reflectors = nullptr;
        sq::Shader* part_ambient_soft = nullptr;
        sq::Shader* part_skylight_soft = nullptr;
        sq::Shader* part_spot_none_soft = nullptr;
        sq::Shader* part_spot_shad_soft = nullptr;
        sq::Shader* part_point_none_soft = nullptr;
        sq::Shader* part_point_shad_soft = nullptr;
        sq::Shader* part_writefinal_soft = nullptr;
    } FS;

    struct {
        sq::Texture2D* ssaoA = nullptr;
        sq::Texture2D* ssaoB = nullptr;
        sq::Texture2D* pshadA = nullptr;
        sq::Texture2D* pshadB = nullptr;
        sq::Texture2D* bloomA = nullptr;
        sq::Texture2D* bloomB = nullptr;
        sq::Texture2D* baseDpSt = nullptr;
        sq::Texture2D* baseDiff = nullptr;
        sq::Texture2D* baseSurf = nullptr;
        sq::Texture2D* baseNorm = nullptr;
        sq::Texture2D* baseSpec = nullptr;
        sq::Texture2D* reflDpSt = nullptr;
        sq::Texture2D* reflDiff = nullptr;
        sq::Texture2D* reflSurf = nullptr;
        sq::Texture2D* partDpSt = nullptr;
        sq::Texture2D* partMain = nullptr;
        sq::Texture2D* depHalf = nullptr;
        sq::Texture2D* depQter = nullptr;
        sq::Texture2D* hdrBase = nullptr;
        sq::Texture2D* hdrRefl = nullptr;
        sq::Texture2D* hdrPart = nullptr;
        sq::Texture2D* simple = nullptr;
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
        int shadMult;
        bool shadFltr;
        float viewDist;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        fvec2 fPixSize;
        fvec2 hPixSize;
        fvec2 qPixSize;
    } INFO;

    const sq::Camera* const camera;
    sq::SettingsMaps* const settings;
    sq::Pipeline* pipeline = nullptr;

private:
    const wcoe::Reflector* crntRflct = nullptr;
    const wcoe::Emitter* crntEmitr = nullptr;
};

}}
