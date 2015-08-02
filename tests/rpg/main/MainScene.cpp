#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMaps.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Camera.hpp>

#include "../rndr/Graph.hpp"
#include "../wcoe/World.hpp"
#include "../wcoe/Cell.hpp"
#include "MainScene.hpp"

using namespace sqt;

MainScene::MainScene(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 24u;

    camera.reset(new sq::Camera(0u));
    pipeline.reset(new sq::Pipeline());
    world.reset(new wcoe::World(camera.get(), settings));
    graph.reset(new rndr::Graph(camera.get(), settings));
    graph->pipeline = pipeline.get();
    graph->world = world.get();

    appBase->cs->add_global(chai::var(camera.get()), "camera");
    appBase->cs->add_global(chai::var(world.get()), "world");
    appBase->cs->add_global(chai::var(graph.get()), "graph");

    camera->pos = {-4.f, -1.f, 3.f};
    camera->dir = {0.7, 0.2, -0.1};
    camera->rmin = 0.1f;
    camera->rmax = 40.f;
    camera->size = {16.f, 10.f};
    camera->fov = 1.f;
    posNext = camera->pos;
    camera->update();

//    world->add_cell("GLOBAL", {0,0,0})->DAT_enabled = true;

    /// Player Model


    /// Add Settings
    settings->add<float>("viewDist", 120.f);
    settings->add<int>("shadQlty", 2); // 0=1024, 1=2048, 2=4096
    settings->add<int>("shadFltr", 2); // 0=Off, 1=Minimal, 2=Full
    settings->add<int>("ssaoMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("shftMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("hdrbMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("fxaaMode", 2); // 0=Off, 1=Low, 2=High
    settings->add<int>("vignMode", 2); // 0=Off, 1=Low, 2=High


    /// Create Textures
    TX.ssaoA.reset(new sq::Texture2D());
    TX.ssaoB.reset(new sq::Texture2D());
    TX.pshadA.reset(new sq::Texture2D());
    TX.pshadB.reset(new sq::Texture2D());
    TX.bloomA.reset(new sq::Texture2D());
    TX.bloomB.reset(new sq::Texture2D());
    TX.shafts.reset(new sq::Texture2D());
    TX.baseDiff.reset(new sq::Texture2D());
    TX.baseSurf.reset(new sq::Texture2D());
    TX.baseNorm.reset(new sq::Texture2D());
    TX.baseSpec.reset(new sq::Texture2D());
    TX.baseDpSt.reset(new sq::Texture2D());
    TX.reflDiff.reset(new sq::Texture2D());
    TX.reflSurf.reset(new sq::Texture2D());
    TX.reflDpSt.reset(new sq::Texture2D());
    TX.partMain.reset(new sq::Texture2D());
    TX.partDpSt.reset(new sq::Texture2D());
    TX.depHalf.reset(new sq::Texture2D());
    TX.depQter.reset(new sq::Texture2D());
    TX.hdrBase.reset(new sq::Texture2D());
    TX.hdrRefl.reset(new sq::Texture2D());
    TX.hdrPart.reset(new sq::Texture2D());
    TX.simple.reset(new sq::Texture2D());

    /// Setup Textures
    TX.ssaoA->create(gl::RED, gl::R8, 1);
    TX.ssaoB->create(gl::RED, gl::R8, 1);
    TX.pshadA->create(gl::RED, gl::R8, 1);
    TX.pshadB->create(gl::RED, gl::R8, 1);
    TX.bloomA->create(gl::RGB, gl::RGB8, 3);
    TX.bloomB->create(gl::RGB, gl::RGB8, 3);
    TX.shafts->create(gl::RED, gl::R16F, 1);
    TX.baseDiff->create(gl::RGB, gl::RGB8, 3);
    TX.baseSurf->create(gl::RGB, gl::RGB12, 3);
    TX.baseNorm->create(gl::RGB, gl::RGB12, 3);
    TX.baseSpec->create(gl::RGB, gl::RGB8, 3);
    TX.baseDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.reflDiff->create(gl::RGB, gl::RGB8, 3);
    TX.reflSurf->create(gl::RGB, gl::RGB8, 3);
    TX.reflDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.partMain->create(gl::RGBA, gl::RGBA16F, 4);
    TX.partDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    TX.depHalf->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1);
    TX.depQter->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1);
    TX.hdrBase->create(gl::RGBA, gl::RGBA16F, 4);
    TX.hdrRefl->create(gl::RGB, gl::RGB16F, 3);
    TX.hdrPart->create(gl::RGBA, gl::RGBA16F, 4);
    TX.simple->create(gl::RGBA, gl::RGBA8, 4);

    /// Set Texture Presets
    TX.ssaoA->set_preset(sq::Texture2D::L_C());
    TX.ssaoB->set_preset(sq::Texture2D::L_C());
    TX.pshadA->set_preset(sq::Texture2D::L_C());
    TX.pshadB->set_preset(sq::Texture2D::L_C());
    TX.bloomA->set_preset(sq::Texture2D::L_C());
    TX.bloomB->set_preset(sq::Texture2D::L_C());
    TX.shafts->set_preset(sq::Texture2D::L_C());
    TX.baseDiff->set_preset(sq::Texture2D::L_C());
    TX.baseSurf->set_preset(sq::Texture2D::L_C());
    TX.baseNorm->set_preset(sq::Texture2D::L_C());
    TX.baseSpec->set_preset(sq::Texture2D::L_C());
    TX.baseDpSt->set_preset(sq::Texture2D::L_C());
    TX.reflDiff->set_preset(sq::Texture2D::L_C());
    TX.reflSurf->set_preset(sq::Texture2D::L_C());
    TX.reflDpSt->set_preset(sq::Texture2D::L_C());
    TX.partMain->set_preset(sq::Texture2D::L_C());
    TX.partDpSt->set_preset(sq::Texture2D::L_C());
    TX.depHalf->set_preset(sq::Texture2D::N_C());
    TX.depQter->set_preset(sq::Texture2D::N_C());
    TX.hdrBase->set_preset(sq::Texture2D::L_C());
    TX.hdrRefl->set_preset(sq::Texture2D::L_C());
    TX.hdrPart->set_preset(sq::Texture2D::L_C());
    TX.simple->set_preset(sq::Texture2D::L_C());

    /// Create Framebuffers
    FB.ssaoA.reset(new sq::FrameBuffer());
    FB.ssaoB.reset(new sq::FrameBuffer());
    FB.pshadA.reset(new sq::FrameBuffer());
    FB.pshadB.reset(new sq::FrameBuffer());
    FB.bloomA.reset(new sq::FrameBuffer());
    FB.bloomB.reset(new sq::FrameBuffer());
    FB.shafts.reset(new sq::FrameBuffer());
    FB.defrBase.reset(new sq::FrameBuffer());
    FB.defrRefl.reset(new sq::FrameBuffer());
    FB.defrPart.reset(new sq::FrameBuffer());
    FB.depHalf.reset(new sq::FrameBuffer());
    FB.depQter.reset(new sq::FrameBuffer());
    FB.hdrBase.reset(new sq::FrameBuffer());
    FB.hdrRefl.reset(new sq::FrameBuffer());
    FB.hdrPart.reset(new sq::FrameBuffer());
    FB.simple.reset(new sq::FrameBuffer());

    /// Setup Framebuffers
    FB.ssaoA->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoA);
    FB.ssaoB->attach(gl::COLOR_ATTACHMENT0, *TX.ssaoB);
    FB.pshadA->attach(gl::COLOR_ATTACHMENT0, *TX.pshadA);
    FB.pshadB->attach(gl::COLOR_ATTACHMENT0, *TX.pshadB);
    FB.bloomA->attach(gl::COLOR_ATTACHMENT0, *TX.bloomA);
    FB.bloomB->attach(gl::COLOR_ATTACHMENT0, *TX.bloomB);
    FB.shafts->attach(gl::COLOR_ATTACHMENT0, *TX.shafts);
    FB.defrBase->attach(gl::COLOR_ATTACHMENT0, *TX.baseDiff);
    FB.defrBase->attach(gl::COLOR_ATTACHMENT1, *TX.baseSurf);
    FB.defrBase->attach(gl::COLOR_ATTACHMENT2, *TX.baseNorm);
    FB.defrBase->attach(gl::COLOR_ATTACHMENT3, *TX.baseSpec);
    FB.defrBase->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.baseDpSt);
    FB.defrRefl->attach(gl::COLOR_ATTACHMENT0, *TX.reflDiff);
    FB.defrRefl->attach(gl::COLOR_ATTACHMENT1, *TX.reflSurf);
    FB.defrRefl->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.reflDpSt);
    FB.defrPart->attach(gl::COLOR_ATTACHMENT0, *TX.partMain);
    FB.defrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);
    FB.depHalf->attach(gl::DEPTH_ATTACHMENT, *TX.depHalf);
    FB.depQter->attach(gl::DEPTH_ATTACHMENT, *TX.depQter);
    FB.hdrBase->attach(gl::COLOR_ATTACHMENT0, *TX.hdrBase);
    FB.hdrBase->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.baseDpSt);
    FB.hdrRefl->attach(gl::COLOR_ATTACHMENT0, *TX.hdrRefl);
    FB.hdrRefl->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.reflDpSt);
    FB.hdrPart->attach(gl::COLOR_ATTACHMENT0, *TX.hdrPart);
    FB.hdrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);
    FB.simple->attach(gl::COLOR_ATTACHMENT0, *TX.simple);

    /// Import GLSL Headers
    preprocs->import_header("headers/blocks/skybox");
    preprocs->import_header("headers/blocks/ambient");
    preprocs->import_header("headers/blocks/skylight");
    preprocs->import_header("headers/blocks/spotlight");
    preprocs->import_header("headers/blocks/pointlight");
    preprocs->import_header("headers/blocks/reflector");
    preprocs->import_header("headers/blocks/mstatic");
    preprocs->import_header("headers/blocks/mskelly");
    preprocs->import_header("headers/blocks/decal");
    preprocs->import_header("headers/shadow/sample_sky");
    preprocs->import_header("headers/shadow/sample_spot");
    preprocs->import_header("headers/shadow/sample_point");

    /// Create Shaders
    VS.gnrc_screen.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_stencil_base.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_stencil_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_statics_base.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_statics_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_skellys_base.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_skellys_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_decals_base.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_decals_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_static.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_skybox_base.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_skybox_refl.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_reflectors.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.part_vertex_soft.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.part_geometry_soft.reset(new sq::Shader(gl::GEOMETRY_SHADER));
    VS.prty_shafts_shafts.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS.gnrc_fillwith.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_lumalpha.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_models_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_models_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_decals_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_decals_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_skybox.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_ambient_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_ambient_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_skylight_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_skylight_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_none_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_none_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_shad_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_shad_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_spec_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_spot_both_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_none_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_none_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_shad_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_shad_refl.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_spec_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_point_both_base.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_reflectors.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_ambient_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_skylight_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_spot_none_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_spot_shad_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_point_none_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_point_shad_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_writefinal_soft.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_shafts_shafts.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_shafts_write.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_ssao.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_ssao_blur.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_highs.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blurh.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_blurv.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_hdr_tones.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_fxaa_fxaa.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.prty_vignette.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VS.gbuf_stencil_base->add_uniform("matrix"); // mat4
    VS.gbuf_stencil_refl->add_uniform("matrix"); // mat4
    FS.gbuf_models_base->add_uniform("d_n_s"); // ivec3
    FS.gbuf_models_refl->add_uniform("d_n_s"); // ivec3
    VS.shad_static->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("matrix"); // mat4

    /// Load Shaders
    VS.gnrc_screen->load(preprocs->load("generic/screen_vs"));
    VS.gbuf_stencil_base->load(preprocs->load("gbuffer/stencil_base_vs"));
    VS.gbuf_stencil_refl->load(preprocs->load("gbuffer/stencil_refl_vs"));
    VS.gbuf_statics_base->load(preprocs->load("gbuffer/statics_base_vs"));
    VS.gbuf_statics_refl->load(preprocs->load("gbuffer/statics_refl_vs"));
    VS.gbuf_skellys_base->load(preprocs->load("gbuffer/skellys_base_vs"));
    VS.gbuf_skellys_refl->load(preprocs->load("gbuffer/skellys_refl_vs"));
    VS.gbuf_decals_base->load(preprocs->load("gbuffer/decals_base_vs"));
    VS.gbuf_decals_refl->load(preprocs->load("gbuffer/decals_refl_vs"));
    VS.shad_static->load(preprocs->load("shadows/static_vs"));
    VS.shad_skelly->load(preprocs->load("shadows/skelly_vs"));
    VS.defr_skybox_base->load(preprocs->load("deferred/skybox_vs"));
    VS.defr_skybox_refl->load(preprocs->load("deferred/skybox_vs", "#define REFLECT"));
    VS.defr_reflectors->load(preprocs->load("deferred/reflectors_vs"));
    VS.part_vertex_soft->load(preprocs->load("particles/vertex_soft_vs"));
    VS.part_geometry_soft->load(preprocs->load("particles/geometry_soft_gs"));
    VS.prty_shafts_shafts->load(preprocs->load("pretty/shafts/shafts_vs"));
    FS.gnrc_fillwith->load(preprocs->load("generic/fillwith_fs"));
    FS.gnrc_lumalpha->load(preprocs->load("generic/lumalpha_fs"));
    FS.gnrc_passthru->load(preprocs->load("generic/passthru_fs"));
    FS.gbuf_models_base->load(preprocs->load("gbuffer/models_base_fs"));
    FS.gbuf_models_refl->load(preprocs->load("gbuffer/models_refl_fs"));
    FS.gbuf_decals_base->load(preprocs->load("gbuffer/decals_base_fs"));
    FS.gbuf_decals_refl->load(preprocs->load("gbuffer/decals_refl_fs"));
    FS.shad_punch->load(preprocs->load("shadows/punch_fs"));
    FS.defr_skybox->load(preprocs->load("deferred/skybox_fs"));
    FS.defr_ambient_refl->load(preprocs->load("deferred/ambient_refl_fs"));
    FS.defr_reflectors->load(preprocs->load("deferred/reflectors_fs"));
    FS.part_ambient_soft->load(preprocs->load("particles/ambient_soft_fs"));
    FS.part_writefinal_soft->load(preprocs->load("particles/writefinal_soft_fs"));
    FS.prty_shafts_write->load(preprocs->load("pretty/shafts/write_fs"));
    FS.prty_hdr_highs->load(preprocs->load("pretty/hdr/highs_fs"));
    FS.prty_vignette->load(preprocs->load("pretty/vignette/vignette_fs"));

    /// Set Graph Vertex Shaders
    graph->VS.gnrc_screen = VS.gnrc_screen.get();
    graph->VS.gbuf_stencil_base = VS.gbuf_stencil_base.get();
    graph->VS.gbuf_stencil_refl = VS.gbuf_stencil_refl.get();
    graph->VS.gbuf_statics_base = VS.gbuf_statics_base.get();
    graph->VS.gbuf_statics_refl = VS.gbuf_statics_refl.get();
    graph->VS.gbuf_skellys_base = VS.gbuf_skellys_base.get();
    graph->VS.gbuf_skellys_refl = VS.gbuf_skellys_refl.get();
    graph->VS.gbuf_decals_base = VS.gbuf_decals_base.get();
    graph->VS.gbuf_decals_refl = VS.gbuf_decals_refl.get();
    graph->VS.shad_static = VS.shad_static.get();
    graph->VS.shad_skelly = VS.shad_skelly.get();
    graph->VS.defr_skybox_base = VS.defr_skybox_base.get();
    graph->VS.defr_skybox_refl = VS.defr_skybox_refl.get();
    graph->VS.defr_reflectors = VS.defr_reflectors.get();
    graph->VS.part_vertex_soft = VS.part_vertex_soft.get();
    graph->VS.part_geometry_soft = VS.part_geometry_soft.get();

    /// Set Graph Fragment Shaders
    graph->FS.gnrc_fillwith = FS.gnrc_fillwith.get();
    graph->FS.gnrc_lumalpha = FS.gnrc_lumalpha.get();
    graph->FS.gnrc_passthru = FS.gnrc_passthru.get();
    graph->FS.gbuf_models_base = FS.gbuf_models_base.get();
    graph->FS.gbuf_models_refl = FS.gbuf_models_refl.get();
    graph->FS.gbuf_decals_base = FS.gbuf_decals_base.get();
    graph->FS.gbuf_decals_refl = FS.gbuf_decals_refl.get();
    graph->FS.shad_punch = FS.shad_punch.get();
    graph->FS.defr_skybox = FS.defr_skybox.get();
    graph->FS.defr_ambient_base = FS.defr_ambient_base.get();
    graph->FS.defr_ambient_refl = FS.defr_ambient_refl.get();
    graph->FS.defr_skylight_base = FS.defr_skylight_base.get();
    graph->FS.defr_skylight_refl = FS.defr_skylight_refl.get();
    graph->FS.defr_spot_none_base = FS.defr_spot_none_base.get();
    graph->FS.defr_spot_none_refl = FS.defr_spot_none_refl.get();
    graph->FS.defr_spot_shad_base = FS.defr_spot_shad_base.get();
    graph->FS.defr_spot_shad_refl = FS.defr_spot_shad_refl.get();
    graph->FS.defr_spot_spec_base = FS.defr_spot_spec_base.get();
    graph->FS.defr_spot_both_base = FS.defr_spot_both_base.get();
    graph->FS.defr_point_none_base = FS.defr_point_none_base.get();
    graph->FS.defr_point_none_refl = FS.defr_point_none_refl.get();
    graph->FS.defr_point_shad_base = FS.defr_point_shad_base.get();
    graph->FS.defr_point_shad_refl = FS.defr_point_shad_refl.get();
    graph->FS.defr_point_spec_base = FS.defr_point_spec_base.get();
    graph->FS.defr_point_both_base = FS.defr_point_both_base.get();
    graph->FS.defr_reflectors = FS.defr_reflectors.get();
    graph->FS.part_ambient_soft = FS.part_ambient_soft.get();
    graph->FS.part_skylight_soft = FS.part_skylight_soft.get();
    graph->FS.part_spot_none_soft = FS.part_spot_none_soft.get();
    graph->FS.part_spot_shad_soft = FS.part_spot_shad_soft.get();
    graph->FS.part_point_none_soft = FS.part_point_none_soft.get();
    graph->FS.part_point_shad_soft = FS.part_point_shad_soft.get();
    graph->FS.part_writefinal_soft = FS.part_writefinal_soft.get();

    /// Set Graph Textures
    graph->TX.ssaoA = TX.ssaoA.get();
    graph->TX.ssaoB = TX.ssaoB.get();
    graph->TX.pshadA = TX.pshadA.get();
    graph->TX.pshadB = TX.pshadB.get();
    graph->TX.bloomA = TX.bloomA.get();
    graph->TX.bloomB = TX.bloomB.get();
    graph->TX.baseDiff = TX.baseDiff.get();
    graph->TX.baseSurf = TX.baseSurf.get();
    graph->TX.baseNorm = TX.baseNorm.get();
    graph->TX.baseSpec = TX.baseSpec.get();
    graph->TX.baseDpSt = TX.baseDpSt.get();
    graph->TX.reflDiff = TX.reflDiff.get();
    graph->TX.reflSurf = TX.reflSurf.get();
    graph->TX.reflDpSt = TX.reflDpSt.get();
    graph->TX.partMain = TX.partMain.get();
    graph->TX.partDpSt = TX.partDpSt.get();
    graph->TX.depHalf = TX.depHalf.get();
    graph->TX.depQter = TX.depQter.get();
    graph->TX.hdrBase = TX.hdrBase.get();
    graph->TX.hdrRefl = TX.hdrRefl.get();
    graph->TX.hdrPart = TX.hdrPart.get();
    graph->TX.simple = TX.simple.get();

    /// Set Graph Framebuffers
    graph->FB.ssaoA = FB.ssaoA.get();
    graph->FB.ssaoB = FB.ssaoB.get();
    graph->FB.pshadA = FB.pshadA.get();
    graph->FB.pshadB = FB.pshadB.get();
    graph->FB.bloomA = FB.bloomA.get();
    graph->FB.bloomB = FB.bloomB.get();
    graph->FB.defrBase = FB.defrBase.get();
    graph->FB.defrRefl = FB.defrRefl.get();
    graph->FB.defrPart = FB.defrPart.get();
    graph->FB.hdrBase = FB.hdrBase.get();
    graph->FB.hdrRefl = FB.hdrRefl.get();
    graph->FB.hdrPart = FB.hdrPart.get();
    graph->FB.simple = FB.simple.get();
}



void MainScene::update() {
    using KB = sf::Keyboard;
    posCrnt = posNext;

    if (focused == true) {
        if (KB::isKeyPressed(KB::PageUp)) posNext.z += 0.05f;
        if (KB::isKeyPressed(KB::PageDown)) posNext.z -= 0.05f;

        if (KB::isKeyPressed(KB::Right) && !KB::isKeyPressed(KB::Left))
            posNext += glm::rotateZ(fvec3(0.08f, 0.f, 0.f), rotZ);
        else if (KB::isKeyPressed(KB::Left) && !KB::isKeyPressed(KB::Right))
            posNext += glm::rotateZ(fvec3(-0.08f, 0.f, 0.f), rotZ);

        if (KB::isKeyPressed(KB::Up) && !KB::isKeyPressed(KB::Down))
            posNext += glm::rotateZ(fvec3(0.f, 0.08f, 0.f), rotZ);
        else if (KB::isKeyPressed(KB::Down) && !KB::isKeyPressed(KB::Up))
            posNext += glm::rotateZ(fvec3(0.f, -0.08f, 0.f), rotZ);
    }

    world->tick();
    graph->update();
}

void MainScene::render(float _ft) {
    if (settings->crnt<bool>("console_active")) focused = false;

    camera->pos = glm::mix(posCrnt, posNext, accum * 24.f);
    if (focused == true) {
        fvec2 mMove = appBase->mouse_relatify();
        rotZ = rotZ + mMove.x / 600.f;
        rotX = glm::clamp(rotX + mMove.y / 900.f, -1.25f, 1.25f);
        camera->dir = glm::rotateZ(glm::rotateX(fvec3(0,1,0), rotX), rotZ);
    }

    camera->update();
    world->calc(accum);


    /// Setup Stuff
    pipeline->bind();
    gl::DepthFunc(gl::LEQUAL);
    TX.baseDiff->bind(gl::TEXTURE3);
    TX.baseSurf->bind(gl::TEXTURE4);
    TX.baseNorm->bind(gl::TEXTURE5);
    TX.baseSpec->bind(gl::TEXTURE6);
    TX.baseDpSt->bind(gl::TEXTURE7);


    /// Light Shadows
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    graph->render_shadows_sky_A();
    graph->render_shadows_sky_B();
    graph->render_shadows_spot();
    graph->render_shadows_point();


    /// Clear G-Buffer
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::STENCIL_ON(); FB.defrBase->use();
    sq::VIEWPORT(INFO.fullSize);
    gl::StencilMask(0b1111);
    gl::ClearStencil(0b0001);
    sq::CLEAR_COLOR_DEPTH_STENC();
    gl::ClearStencil(0b0000);


    /// Render Models into G-Buffer
    graph->render_mstatics_base(true);
    graph->render_mskellys_base(true);
    graph->render_reflects_base(true);
    graph->render_decals_base();
    graph->render_mstatics_base(false);
    graph->render_mskellys_base(false);
    graph->render_reflects_base(false);


    /// Render Downscaled Depth Buffers
    FB.defrBase->bind(gl::READ_FRAMEBUFFER); FB.depHalf->bind(gl::DRAW_FRAMEBUFFER);
    sq::BLIT_FRAMEBUFFER(INFO.fullSize, INFO.halfSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);
    FB.defrBase->bind(gl::READ_FRAMEBUFFER); FB.depQter->bind(gl::DRAW_FRAMEBUFFER);
    sq::BLIT_FRAMEBUFFER(INFO.fullSize, INFO.qterSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);


    /// Render SSAO Texture
    if (INFO.ssaoEnable == true) {
        TX.depHalf->bind(gl::TEXTURE0);
        sq::DEPTH_OFF(); sq::STENCIL_OFF();
        sq::BLEND_OFF(); sq::CULLFACE_OFF();
        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*FS.prty_ssao_ssao);
        FB.ssaoA->use(); sq::VIEWPORT(INFO.halfSize);
        sq::draw_screen_quad(); TX.ssaoA->bind(gl::TEXTURE0);
        pipeline->use_shader(*FS.prty_ssao_blur);
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
        FB.ssaoA->use(); sq::draw_screen_quad(); TX.ssaoA->bind();
        FB.ssaoB->use(); sq::draw_screen_quad(); TX.ssaoB->bind();
    }


    /// Clear HDR Framebuffer
    sq::VIEWPORT(INFO.fullSize);
    FB.hdrBase->use(); sq::CLEAR_COLOR();
    sq::DEPTH_OFF(); sq::STENCIL_ON();
    sq::CULLFACE_OFF();


    /// Render Lights, Reflectors, Emitters
    graph->render_skybox_base();
    graph->render_ambient_base();
    graph->render_skylight_base();
    graph->render_spotlights_base();
    graph->render_pointlights_base();
    graph->render_reflections();
    graph->render_particles();


    /// Render Light Shafts Texture
    if (INFO.shftEnable == true) {
        world->skylight.ubo->bind(1u);
        sq::DEPTH_OFF(); sq::BLEND_OFF();
        sq::STENCIL_OFF();

        sq::VIEWPORT(INFO.qterSize);
        FB.shafts->use(); sq::CLEAR_COLOR();
        pipeline->use_shader(*VS.prty_shafts_shafts);
        pipeline->use_shader(*FS.prty_shafts_shafts);
        world->skylight.texA->bind(gl::TEXTURE0);
        TX.depQter->bind(gl::TEXTURE1);
        sq::draw_screen_quad();

        FB.hdrBase->use();
        sq::VIEWPORT(INFO.fullSize);
        TX.shafts->bind(gl::TEXTURE0);
        TX.baseDpSt->bind(gl::TEXTURE2);
        sq::BLEND_ON(); sq::BLEND_ONEONE();
        pipeline->use_shader(*VS.gnrc_screen);
        pipeline->use_shader(*FS.prty_shafts_write);
        sq::draw_screen_quad();
    }


    /// Write HDR Luma to Alpha
    sq::DEPTH_OFF(); sq::STENCIL_OFF();
    sq::BLEND_OFF(); sq::CULLFACE_OFF();
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.gnrc_lumalpha);
    FB.hdrBase->use(); sq::VIEWPORT(INFO.fullSize);
    TX.hdrBase->bind(gl::TEXTURE0);
    sq::draw_screen_quad();


    /// Render Bloom Texture
    if (INFO.hdrbEnable == true) {
        sq::VIEWPORT(INFO.qterSize);
        gl::ActiveTexture(gl::TEXTURE1);
        pipeline->use_shader(*FS.prty_hdr_highs); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind();
        pipeline->use_shader(*FS.prty_hdr_blurh); FB.bloomB->use();
        sq::draw_screen_quad(); TX.bloomB->bind();
        pipeline->use_shader(*FS.prty_hdr_blurv); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind();
        pipeline->use_shader(*FS.prty_hdr_blurh); FB.bloomB->use();
        sq::draw_screen_quad(); TX.bloomB->bind();
        pipeline->use_shader(*FS.prty_hdr_blurv); FB.bloomA->use();
        sq::draw_screen_quad(); TX.bloomA->bind();
    }


    /// HDR Tonemapping
    sq::STENCIL_OFF();
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.prty_hdr_tones);
    FB.simple->use(); sq::VIEWPORT(INFO.fullSize);
    TX.hdrBase->bind(gl::TEXTURE0);
    sq::draw_screen_quad();


    /// Antialiasing and Write to Screen
    TX.simple->bind(gl::TEXTURE0);
    if (INFO.fxaaEnable == true) {
        pipeline->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        pipeline->use_shader(*FS.prty_fxaa_fxaa);
    } else pipeline->use_shader(*FS.gnrc_passthru);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();


    /// Vignetting Effect
    if (INFO.vgntEnable == true) {
        pipeline->use_shader(*FS.prty_vignette);
        sq::BLEND_ON(); sq::BLEND_ALPHA();
        sq::draw_screen_quad();
    }

    gl::BindProgramPipeline(0);
    gl::BindVertexArray(0);
}


void MainScene::update_settings() {
    INFO.fullSize = appBase->get_size();
    INFO.halfSize = INFO.fullSize / 2u;
    INFO.qterSize = INFO.fullSize / 4u;
    INFO.fPixSize = 1.f / fvec2(INFO.fullSize);
    INFO.hPixSize = 1.f / fvec2(INFO.halfSize);
    INFO.qPixSize = 1.f / fvec2(INFO.qterSize);

    graph->INFO.fullSize = INFO.fullSize;
    graph->INFO.halfSize = INFO.halfSize;
    graph->INFO.qterSize = INFO.qterSize;
    graph->INFO.fPixSize = INFO.fPixSize;
    graph->INFO.hPixSize = INFO.hPixSize;
    graph->INFO.qPixSize = INFO.qPixSize;

    TX.ssaoA->resize(INFO.halfSize);
    TX.ssaoB->resize(INFO.halfSize);
    TX.pshadA->resize(INFO.halfSize);
    TX.pshadB->resize(INFO.halfSize);
    TX.bloomA->resize(INFO.qterSize);
    TX.bloomB->resize(INFO.qterSize);
    TX.shafts->resize(INFO.qterSize);
    TX.baseDiff->resize(INFO.fullSize);
    TX.baseSurf->resize(INFO.fullSize);
    TX.baseNorm->resize(INFO.fullSize);
    TX.baseSpec->resize(INFO.fullSize);
    TX.baseDpSt->resize(INFO.fullSize);
    TX.reflDiff->resize(INFO.halfSize);
    TX.reflSurf->resize(INFO.halfSize);
    TX.reflDpSt->resize(INFO.halfSize);
    TX.partMain->resize(INFO.halfSize);
    TX.partDpSt->resize(INFO.halfSize);
    TX.depHalf->resize(INFO.halfSize);
    TX.depQter->resize(INFO.qterSize);
    TX.hdrBase->resize(INFO.fullSize);
    TX.hdrRefl->resize(INFO.halfSize);
    TX.hdrPart->resize(INFO.halfSize);
    TX.simple->resize(INFO.fullSize);

    camera->size = fvec2(INFO.fullSize);

    INFO.viewDist = float(settings->crnt<float>("viewDist"));
    INFO.ssaoEnable = bool(settings->crnt<int>("ssaoMode"));
    INFO.shftEnable = bool(settings->crnt<int>("shftMode"));
    INFO.hdrbEnable = bool(settings->crnt<int>("hdrbMode"));
    INFO.fxaaEnable = bool(settings->crnt<int>("fxaaMode"));
    INFO.vgntEnable = bool(settings->crnt<int>("vignMode"));
    INFO.shadMult = uint(glm::pow(2, settings->crnt<int>("shadQlty")));
    INFO.shadFltr = bool(settings->crnt<int>("shadFltr"));

    graph->INFO.viewDist = INFO.viewDist;
    graph->INFO.shadMult = INFO.shadMult;
    graph->INFO.shadFltr = INFO.shadFltr;
    camera->rmax = INFO.viewDist;

    graph->update_settings();
    reload_shaders();
}


void MainScene::reload_shaders() {
    int shadFltr = settings->crnt<int>("shadFltr");
    int shadQlty = settings->crnt<int>("shadQlty");
    int ssaoMode = settings->crnt<int>("ssaoMode");
    int hdrbMode = settings->crnt<int>("hdrbMode");
    int shftMode = settings->crnt<int>("shftMode");
    int fxaaMode = settings->crnt<int>("fxaaMode");

    /// Lighting
    string defines = "#define SHADFLTR " + std::to_string(shadFltr) + "\n"
                   + "#define SHADQLTY " + std::to_string(shadQlty);
    string definesShad = defines + "\n#define SHADOW";
    string definesSpec = defines + "\n#define SPECULAR";
    string definesBoth = defines + "\n#define SHADOW\n#define SPECULAR";
    FS.defr_skylight_base->load(preprocs->load("deferred/skylight_base_fs", defines));
    FS.defr_skylight_refl->load(preprocs->load("deferred/skylight_refl_fs", defines));
    FS.defr_spot_none_base->load(preprocs->load("deferred/spotlight_base_fs", defines));
    FS.defr_spot_none_refl->load(preprocs->load("deferred/spotlight_refl_fs", defines));
    FS.defr_spot_shad_base->load(preprocs->load("deferred/spotlight_base_fs", definesShad));
    FS.defr_spot_shad_refl->load(preprocs->load("deferred/spotlight_refl_fs", definesShad));
    FS.defr_spot_spec_base->load(preprocs->load("deferred/spotlight_base_fs", definesSpec));
    FS.defr_spot_both_base->load(preprocs->load("deferred/spotlight_base_fs", definesBoth));
    FS.defr_point_none_base->load(preprocs->load("deferred/pointlight_base_fs", defines));
    FS.defr_point_none_refl->load(preprocs->load("deferred/pointlight_refl_fs", defines));
    FS.defr_point_shad_base->load(preprocs->load("deferred/pointlight_base_fs", definesShad));
    FS.defr_point_shad_refl->load(preprocs->load("deferred/pointlight_refl_fs", definesShad));
    FS.defr_point_spec_base->load(preprocs->load("deferred/pointlight_base_fs", definesSpec));
    FS.defr_point_both_base->load(preprocs->load("deferred/pointlight_base_fs", definesBoth));
    FS.part_skylight_soft->load(preprocs->load("particles/skylight_soft_fs", defines));
    FS.part_spot_none_soft->load(preprocs->load("particles/spotlight_soft_fs", defines));
    FS.part_spot_shad_soft->load(preprocs->load("particles/spotlight_soft_fs", definesShad));
    FS.part_point_none_soft->load(preprocs->load("particles/pointlight_soft_fs", defines));
    FS.part_point_shad_soft->load(preprocs->load("particles/pointlight_soft_fs", definesShad));

    /// SSAO
    defines = "#define PIXSIZE " + glm::to_string(INFO.hPixSize);
    if (ssaoMode == 0) FS.defr_ambient_base->load(preprocs->load("deferred/ambient_base_fs"));
    else FS.defr_ambient_base->load(preprocs->load("deferred/ambient_base_fs", "#define SSAO"));
    if (ssaoMode == 1) FS.prty_ssao_ssao->load(preprocs->load("pretty/ssao/ssao_fs", defines)),
                       FS.prty_ssao_blur->load(preprocs->load("pretty/ssao/blur_fs", defines));
    if (ssaoMode == 2) FS.prty_ssao_ssao->load(preprocs->load("pretty/ssao/ssao_fs", defines+"\n#define HIGH")),
                       FS.prty_ssao_blur->load(preprocs->load("pretty/ssao/blur_fs", defines+"\n#define HIGH"));

    /// Shafts
    if (shftMode == 1) FS.prty_shafts_shafts->load(preprocs->load("pretty/shafts/shafts_fs"));
    if (shftMode == 2) FS.prty_shafts_shafts->load(preprocs->load("pretty/shafts/shafts_fs", "#define HIGH"));

    /// Bloom
    defines = "#define PIXSIZE " + glm::to_string(INFO.qPixSize);
    if (hdrbMode == 1) FS.prty_hdr_blurh->load(preprocs->load("pretty/hdr/blurh_fs", defines)),
                       FS.prty_hdr_blurv->load(preprocs->load("pretty/hdr/blurv_fs", defines));
    if (hdrbMode == 2) FS.prty_hdr_blurh->load(preprocs->load("pretty/hdr/blurh_fs", defines+"\n#define HIGH")),
                       FS.prty_hdr_blurv->load(preprocs->load("pretty/hdr/blurv_fs", defines+"\n#define HIGH"));

    /// HDR
    if (hdrbMode == 0) FS.prty_hdr_tones->load(preprocs->load("pretty/hdr/tones_fs"));
    else FS.prty_hdr_tones->load(preprocs->load("pretty/hdr/tones_fs", "#define HDRB"));

    /// FXAA
    defines = "#define PIXSIZE " + glm::to_string(INFO.fPixSize);
    if (fxaaMode == 1) FS.prty_fxaa_fxaa->load(preprocs->load("pretty/fxaa/fxaa_fs", defines));
    if (fxaaMode == 2) FS.prty_fxaa_fxaa->load(preprocs->load("pretty/fxaa/fxaa_fs", defines+"\n#define HIGH"));
}
