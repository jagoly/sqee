#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>

namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Graph; }

class MainScene : public sq::Scene {
public:
    MainScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::Camera> camera;
    unique_ptr<wcoe::World> world;
    unique_ptr<rndr::Graph> graph;

    fvec3 posCrnt, posNext;
    float rotX =0, rotZ =0;

    struct {
        unique_ptr<sq::Shader> gnrc_screen;
        unique_ptr<sq::Shader> gbuf_base_stencil;
        unique_ptr<sq::Shader> gbuf_base_simple;
        unique_ptr<sq::Shader> gbuf_base_skelly;
        unique_ptr<sq::Shader> gbuf_base_decal;
        unique_ptr<sq::Shader> gbuf_refl_stencil;
        unique_ptr<sq::Shader> gbuf_refl_simple;
        unique_ptr<sq::Shader> gbuf_refl_skelly;
        unique_ptr<sq::Shader> gbuf_refl_decal;
        unique_ptr<sq::Shader> shad_simple;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> irrd_gbuf_simple;
        unique_ptr<sq::Shader> irrd_defr_skybox;
        unique_ptr<sq::Shader> defr_reflector;
        unique_ptr<sq::Shader> defr_base_skybox;
        unique_ptr<sq::Shader> defr_refl_skybox;
        unique_ptr<sq::Shader> part_soft_vertex;
        unique_ptr<sq::Shader> prty_shafts_shafts;
        unique_ptr<sq::Shader> dbug_envsphere;
    } VS;

    struct {
        unique_ptr<sq::Shader> part_soft_geometry;
    } GS;

    struct {
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> gbuf_base_model;
        unique_ptr<sq::Shader> gbuf_base_decal;
        unique_ptr<sq::Shader> gbuf_refl_model;
        unique_ptr<sq::Shader> gbuf_refl_decal;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> irrd_gbuf_model;
        unique_ptr<sq::Shader> irrd_defr_skybox;
        unique_ptr<sq::Shader> irrd_defr_skylight;
        unique_ptr<sq::Shader> irrd_defr_spotlight;
        unique_ptr<sq::Shader> irrd_defr_pointlight;
        unique_ptr<sq::Shader> irrd_harmonics_shm;
        unique_ptr<sq::Shader> irrd_harmonics_sdv;
        unique_ptr<sq::Shader> defr_skybox;
        unique_ptr<sq::Shader> defr_reflector;
        unique_ptr<sq::Shader> defr_base_ambient;
        unique_ptr<sq::Shader> defr_base_skylight;
        unique_ptr<sq::Shader> defr_base_spot_none;
        unique_ptr<sq::Shader> defr_base_spot_shad;
        unique_ptr<sq::Shader> defr_base_spot_spec;
        unique_ptr<sq::Shader> defr_base_spot_both;
        unique_ptr<sq::Shader> defr_base_point_none;
        unique_ptr<sq::Shader> defr_base_point_shad;
        unique_ptr<sq::Shader> defr_base_point_spec;
        unique_ptr<sq::Shader> defr_base_point_both;
        unique_ptr<sq::Shader> defr_refl_ambient;
        unique_ptr<sq::Shader> defr_refl_skylight;
        unique_ptr<sq::Shader> defr_refl_spot_none;
        unique_ptr<sq::Shader> defr_refl_spot_shad;
        unique_ptr<sq::Shader> defr_refl_point_none;
        unique_ptr<sq::Shader> defr_refl_point_shad;
        unique_ptr<sq::Shader> part_soft_ambient;
        unique_ptr<sq::Shader> part_soft_skylight;
        unique_ptr<sq::Shader> part_soft_spot_none;
        unique_ptr<sq::Shader> part_soft_spot_shad;
        unique_ptr<sq::Shader> part_soft_point_none;
        unique_ptr<sq::Shader> part_soft_point_shad;
        unique_ptr<sq::Shader> part_soft_write;
        unique_ptr<sq::Shader> prty_shafts_shafts;
        unique_ptr<sq::Shader> prty_shafts_write;
        unique_ptr<sq::Shader> prty_ssao_ssao;
        unique_ptr<sq::Shader> prty_ssao_blur;
        unique_ptr<sq::Shader> prty_hdr_highs;
        unique_ptr<sq::Shader> prty_hdr_blurh;
        unique_ptr<sq::Shader> prty_hdr_blurv;
        unique_ptr<sq::Shader> prty_hdr_tones;
        unique_ptr<sq::Shader> prty_fxaa_fxaa;
        unique_ptr<sq::Shader> prty_vignette;
        unique_ptr<sq::Shader> dbug_envsphere_shm;
        unique_ptr<sq::Shader> dbug_envsphere_sdv;
    } FS;

    struct {
        unique_ptr<sq::TextureMut2D> ssaoA;
        unique_ptr<sq::TextureMut2D> ssaoB;
        unique_ptr<sq::TextureMut2D> pshadA;
        unique_ptr<sq::TextureMut2D> pshadB;
        unique_ptr<sq::TextureMut2D> bloomA;
        unique_ptr<sq::TextureMut2D> bloomB;
        unique_ptr<sq::TextureMut2D> shafts;
        unique_ptr<sq::TextureMut2D> backDep;
        unique_ptr<sq::TextureMut2D> baseDiff;
        unique_ptr<sq::TextureMut2D> baseSurf;
        unique_ptr<sq::TextureMut2D> baseNorm;
        unique_ptr<sq::TextureMut2D> baseSpec;
        unique_ptr<sq::TextureMut2D> baseDpSt;
        unique_ptr<sq::TextureMut2D> reflDiff;
        unique_ptr<sq::TextureMut2D> reflSurf;
        unique_ptr<sq::TextureMut2D> reflDpSt;
        unique_ptr<sq::TextureMut2D> partMain;
        unique_ptr<sq::TextureMut2D> partDpSt;
        unique_ptr<sq::TextureMut2D> depHalf;
        unique_ptr<sq::TextureMut2D> depQter;
        unique_ptr<sq::TextureMut2D> hdrBase;
        unique_ptr<sq::TextureMut2D> hdrRefl;
        unique_ptr<sq::TextureMut2D> hdrPart;
        unique_ptr<sq::TextureMut2D> simple;
    } TX;

    struct {
        unique_ptr<sq::FrameBuffer> ssaoA;
        unique_ptr<sq::FrameBuffer> ssaoB;
        unique_ptr<sq::FrameBuffer> pshadA;
        unique_ptr<sq::FrameBuffer> pshadB;
        unique_ptr<sq::FrameBuffer> bloomA;
        unique_ptr<sq::FrameBuffer> bloomB;
        unique_ptr<sq::FrameBuffer> shafts;
        unique_ptr<sq::FrameBuffer> backDep;
        unique_ptr<sq::FrameBuffer> defrBase;
        unique_ptr<sq::FrameBuffer> defrRefl;
        unique_ptr<sq::FrameBuffer> defrPart;
        unique_ptr<sq::FrameBuffer> depHalf;
        unique_ptr<sq::FrameBuffer> depQter;
        unique_ptr<sq::FrameBuffer> hdrBase;
        unique_ptr<sq::FrameBuffer> hdrRefl;
        unique_ptr<sq::FrameBuffer> hdrPart;
        unique_ptr<sq::FrameBuffer> simple;
    } FB;

    struct {
        float viewdist;
        bool shadfilter;
        bool shadlarge;
        bool vignette;
        bool bloom;
        bool irrd;
        int shafts;
        int ssao;
        int fxaa;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        fvec2 fPixSize;
        fvec2 hPixSize;
        fvec2 qPixSize;
        float aspect;
    } INFO;

    void reload_shaders();
};

}
