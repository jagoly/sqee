#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/scene.hpp>

namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Graph; }

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Application* _app);

    void update();
    void render(float _ft);
    void update_settings();

    bool focused = false;

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::Camera> camera;
    unique_ptr<wcoe::World> world;
    unique_ptr<rndr::Graph> graph;

    sq::Direction moveDir;
    fvec3 posCrnt, posNext;
    float rotX, rotZ;

    struct {
        unique_ptr<sq::Shader> gnrc_screen;
        unique_ptr<sq::Shader> gbuf_stencil_base;
        unique_ptr<sq::Shader> gbuf_stencil_refl;
        unique_ptr<sq::Shader> gbuf_statics_base;
        unique_ptr<sq::Shader> gbuf_statics_refl;
        unique_ptr<sq::Shader> gbuf_skellys_base;
        unique_ptr<sq::Shader> gbuf_skellys_refl;
        unique_ptr<sq::Shader> gbuf_decals_base;
        unique_ptr<sq::Shader> gbuf_decals_refl;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> defr_skybox_base;
        unique_ptr<sq::Shader> defr_skybox_refl;
        unique_ptr<sq::Shader> defr_reflectors;
        unique_ptr<sq::Shader> part_vertex_soft;
        unique_ptr<sq::Shader> part_geometry_soft;
    } VS;

    struct {
        unique_ptr<sq::Shader> gnrc_fillwith;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> gbuf_models_base;
        unique_ptr<sq::Shader> gbuf_models_refl;
        unique_ptr<sq::Shader> gbuf_decals_base;
        unique_ptr<sq::Shader> gbuf_decals_refl;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> defr_skybox;
        unique_ptr<sq::Shader> defr_ambient_base;
        unique_ptr<sq::Shader> defr_ambient_refl;
        unique_ptr<sq::Shader> defr_skylight_base;
        unique_ptr<sq::Shader> defr_skylight_refl;
        unique_ptr<sq::Shader> defr_spot_none_base;
        unique_ptr<sq::Shader> defr_spot_none_refl;
        unique_ptr<sq::Shader> defr_spot_shad_base;
        unique_ptr<sq::Shader> defr_spot_shad_refl;
        unique_ptr<sq::Shader> defr_spot_spec_base;
        unique_ptr<sq::Shader> defr_spot_both_base;
        unique_ptr<sq::Shader> defr_point_none_base;
        unique_ptr<sq::Shader> defr_point_none_refl;
        unique_ptr<sq::Shader> defr_point_shad_base;
        unique_ptr<sq::Shader> defr_point_shad_refl;
        unique_ptr<sq::Shader> defr_point_spec_base;
        unique_ptr<sq::Shader> defr_point_both_base;
        unique_ptr<sq::Shader> defr_reflectors;
        unique_ptr<sq::Shader> part_ambient_soft;
        unique_ptr<sq::Shader> part_skylight_soft;
        unique_ptr<sq::Shader> part_spot_none_soft;
        unique_ptr<sq::Shader> part_spot_shad_soft;
        unique_ptr<sq::Shader> part_point_none_soft;
        unique_ptr<sq::Shader> part_point_shad_soft;
        unique_ptr<sq::Shader> part_writefinal_soft;
        unique_ptr<sq::Shader> prty_ssao_ssao;
        unique_ptr<sq::Shader> prty_ssao_blur;
        unique_ptr<sq::Shader> prty_hdr_highs;
        unique_ptr<sq::Shader> prty_hdr_blurh;
        unique_ptr<sq::Shader> prty_hdr_blurv;
        unique_ptr<sq::Shader> prty_hdr_tones;
        unique_ptr<sq::Shader> prty_fxaa_fxaa;
        unique_ptr<sq::Shader> prty_vignette;
    } FS;

    struct {
        unique_ptr<sq::Texture2D> ssaoA;
        unique_ptr<sq::Texture2D> ssaoB;
        unique_ptr<sq::Texture2D> pshadA;
        unique_ptr<sq::Texture2D> pshadB;
        unique_ptr<sq::Texture2D> bloomA;
        unique_ptr<sq::Texture2D> bloomB;
        unique_ptr<sq::Texture2D> baseDiff;
        unique_ptr<sq::Texture2D> baseSurf;
        unique_ptr<sq::Texture2D> baseNorm;
        unique_ptr<sq::Texture2D> baseSpec;
        unique_ptr<sq::Texture2D> baseDpSt;
        unique_ptr<sq::Texture2D> reflDiff;
        unique_ptr<sq::Texture2D> reflSurf;
        unique_ptr<sq::Texture2D> reflDpSt;
        unique_ptr<sq::Texture2D> partMain;
        unique_ptr<sq::Texture2D> partDpSt;
        unique_ptr<sq::Texture2D> hdrBase;
        unique_ptr<sq::Texture2D> hdrRefl;
        unique_ptr<sq::Texture2D> hdrPart;
        unique_ptr<sq::Texture2D> simple;
    } TX;

    struct {
        unique_ptr<sq::Framebuffer> ssaoA;
        unique_ptr<sq::Framebuffer> ssaoB;
        unique_ptr<sq::Framebuffer> pshadA;
        unique_ptr<sq::Framebuffer> pshadB;
        unique_ptr<sq::Framebuffer> bloomA;
        unique_ptr<sq::Framebuffer> bloomB;
        unique_ptr<sq::Framebuffer> defrBase;
        unique_ptr<sq::Framebuffer> defrRefl;
        unique_ptr<sq::Framebuffer> defrPart;
        unique_ptr<sq::Framebuffer> hdrBase;
        unique_ptr<sq::Framebuffer> hdrRefl;
        unique_ptr<sq::Framebuffer> hdrPart;
        unique_ptr<sq::Framebuffer> simple;
    } FB;

    struct {
        bool ssaoEnable;
        bool hdrbEnable;
        bool fxaaEnable;
        bool vgntEnable;
        bool shadFltr;
        uint shadMult;
        float viewDist;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        fvec2 fPixSize;
        fvec2 hPixSize;
        fvec2 qPixSize;
    } INFO;

    void reload_shaders();
};

}
