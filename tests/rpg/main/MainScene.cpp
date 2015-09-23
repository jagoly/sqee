#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/scripts/ChaiScript.hpp>
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

#include <sqee/debug/OpenGL.hpp>

using namespace sqt;

MainScene::MainScene(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 24u;

    camera.reset(new sq::Camera(0u));
    pipeline.reset(new sq::Pipeline());
    world.reset(new wcoe::World(app->settings, camera.get()));
    graph.reset(new rndr::Graph(app->settings, camera.get()));
    graph->pipeline = pipeline.get();
    graph->world = world.get();

    app->cs->add_global(chai::var(camera.get()), "camera");
    app->cs->add_global(chai::var(world.get()), "world");
    app->cs->add_global(chai::var(graph.get()), "graph");

    camera->pos = {-4.f, -1.f, 3.f};
    camera->dir = {0.7, 0.2, -0.1};
    camera->rmin = 0.1f;
    camera->rmax = 40.f;
    camera->size = {16.f, 10.f};
    camera->fov = 1.f;
    posNext = camera->pos;
    camera->update();

    app->console.onHideFuncs.append("MainScene_reset_mouse",
        std::bind<void>(&sq::Application::mouse_centre, app));

    /// Add Settings
    app->settings.add<float>("rpg_viewdist", 120.f);
    app->settings.add<bool>("rpg_shadfilter", true);
    app->settings.add<bool>("rpg_shadlarge", true);
    app->settings.add<bool>("rpg_vignette", true);
    app->settings.add<bool>("rpg_bloom", true);
    app->settings.add<int>("rpg_shafts", 2);
    app->settings.add<int>("rpg_ssao", 2);
    app->settings.add<int>("rpg_fxaa", 2);
    app->settings.add<bool>("rpg_irrd", false);


    /// Create Textures
    TX.ssaoA.reset(new sq::TextureMut2D());
    TX.ssaoB.reset(new sq::TextureMut2D());
    TX.pshadA.reset(new sq::TextureMut2D());
    TX.pshadB.reset(new sq::TextureMut2D());
    TX.bloomA.reset(new sq::TextureMut2D());
    TX.bloomB.reset(new sq::TextureMut2D());
    TX.shafts.reset(new sq::TextureMut2D());
    TX.backDep.reset(new sq::TextureMut2D());
    TX.baseDiff.reset(new sq::TextureMut2D());
    TX.baseSurf.reset(new sq::TextureMut2D());
    TX.baseNorm.reset(new sq::TextureMut2D());
    TX.baseSpec.reset(new sq::TextureMut2D());
    TX.baseDpSt.reset(new sq::TextureMut2D());
    TX.reflDiff.reset(new sq::TextureMut2D());
    TX.reflSurf.reset(new sq::TextureMut2D());
    TX.reflDpSt.reset(new sq::TextureMut2D());
    TX.partMain.reset(new sq::TextureMut2D());
    TX.partDpSt.reset(new sq::TextureMut2D());
    TX.depHalf.reset(new sq::TextureMut2D());
    TX.depQter.reset(new sq::TextureMut2D());
    TX.hdrBase.reset(new sq::TextureMut2D());
    TX.hdrRefl.reset(new sq::TextureMut2D());
    TX.hdrPart.reset(new sq::TextureMut2D());
    TX.simple.reset(new sq::TextureMut2D());

    /// Setup Textures
    TX.ssaoA->create(gl::RED, gl::R8, 1u);
    TX.ssaoB->create(gl::RED, gl::R8, 1u);
    TX.pshadA->create(gl::RED, gl::R8, 1u);
    TX.pshadB->create(gl::RED, gl::R8, 1u);
    TX.bloomA->create(gl::RGB, gl::RGB8, 3u);
    TX.bloomB->create(gl::RGB, gl::RGB8, 3u);
    TX.shafts->create(gl::RED, gl::R16F, 1u);
    TX.backDep->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1u);
    TX.baseDiff->create(gl::RGB, gl::RGB8, 3u);
    TX.baseSurf->create(gl::RGB, gl::RGB10, 3u);
    TX.baseNorm->create(gl::RGB, gl::RGB10, 3u);
    TX.baseSpec->create(gl::RGB, gl::RGB8, 3u);
    TX.baseDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u);
    TX.reflDiff->create(gl::RGB, gl::RGB8, 3u);
    TX.reflSurf->create(gl::RGB, gl::RGB8, 3u);
    TX.reflDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u);
    TX.partMain->create(gl::RGBA, gl::RGBA16F, 4u);
    TX.partDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u);
    TX.depHalf->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1u);
    TX.depQter->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT24, 1u);
    TX.hdrBase->create(gl::RGBA, gl::RGBA16F, 4u);
    TX.hdrRefl->create(gl::RGB, gl::RGB16F, 3u);
    TX.hdrPart->create(gl::RGBA, gl::RGBA16F, 4u);
    TX.simple->create(gl::RGBA, gl::RGBA8, 4u);

    /// Set Texture Presets
    TX.ssaoA->set_preset(sq::Texture::LinearClamp());
    TX.ssaoB->set_preset(sq::Texture::LinearClamp());
    TX.pshadA->set_preset(sq::Texture::LinearClamp());
    TX.pshadB->set_preset(sq::Texture::LinearClamp());
    TX.bloomA->set_preset(sq::Texture::LinearClamp());
    TX.bloomB->set_preset(sq::Texture::LinearClamp());
    TX.shafts->set_preset(sq::Texture::LinearClamp());
    TX.baseDiff->set_preset(sq::Texture::LinearClamp());
    TX.baseSurf->set_preset(sq::Texture::LinearClamp());
    TX.baseNorm->set_preset(sq::Texture::LinearClamp());
    TX.baseSpec->set_preset(sq::Texture::LinearClamp());
    TX.baseDpSt->set_preset(sq::Texture::LinearClamp());
    TX.reflDiff->set_preset(sq::Texture::LinearClamp());
    TX.reflSurf->set_preset(sq::Texture::LinearClamp());
    TX.reflDpSt->set_preset(sq::Texture::LinearClamp());
    TX.partMain->set_preset(sq::Texture::LinearClamp());
    TX.partDpSt->set_preset(sq::Texture::LinearClamp());
    TX.depHalf->set_preset(sq::Texture::LinearClamp());
    TX.depQter->set_preset(sq::Texture::LinearClamp());
    TX.hdrBase->set_preset(sq::Texture::LinearClamp());
    TX.hdrRefl->set_preset(sq::Texture::LinearClamp());
    TX.hdrPart->set_preset(sq::Texture::LinearClamp());
    TX.simple->set_preset(sq::Texture::LinearClamp());

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
    app->preprocs.import_header("headers/blocks/skybox");
    app->preprocs.import_header("headers/blocks/ambient");
    app->preprocs.import_header("headers/blocks/skylight");
    app->preprocs.import_header("headers/blocks/spotlight");
    app->preprocs.import_header("headers/blocks/pointlight");
    app->preprocs.import_header("headers/blocks/reflector");
    app->preprocs.import_header("headers/blocks/msimple");
    app->preprocs.import_header("headers/blocks/mskelly");
    app->preprocs.import_header("headers/blocks/decal");
    app->preprocs.import_header("headers/shadow/sample_sky");
    app->preprocs.import_header("headers/shadow/sample_spot");
    app->preprocs.import_header("headers/shadow/sample_point");
    app->preprocs.import_header("headers/blocks/lightprobe");

    /// Create Vertex Shaders
    VS.gnrc_screen.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_base_stencil.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_base_simple.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_base_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_base_decal.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_refl_stencil.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_refl_simple.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_refl_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.gbuf_refl_decal.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_simple.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.shad_skelly.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.irrd_gbuf_simple.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.irrd_defr_skybox.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_reflector.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_base_skybox.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.defr_refl_skybox.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.part_soft_vertex.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.prty_shafts_shafts.reset(new sq::Shader(gl::VERTEX_SHADER));
    VS.dbug_envsphere.reset(new sq::Shader(gl::VERTEX_SHADER));

    /// Create Geometry Shaders
    GS.part_soft_geometry.reset(new sq::Shader(gl::GEOMETRY_SHADER));

    /// Create Fragment Shaders
    FS.gnrc_passthru.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gnrc_lumalpha.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_base_model.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_base_decal.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_refl_model.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.gbuf_refl_decal.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_gbuf_model.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_defr_skybox.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_defr_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_defr_spotlight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_defr_pointlight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_harmonics_shm.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.irrd_harmonics_sdv.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.shad_punch.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_skybox.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_reflector.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_spot_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_spot_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_spot_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_spot_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_point_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_point_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_point_spec.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_base_point_both.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_spot_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_spot_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_point_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.defr_refl_point_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_ambient.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_skylight.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_spot_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_spot_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_point_none.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_point_shad.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.part_soft_write.reset(new sq::Shader(gl::FRAGMENT_SHADER));
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
    FS.dbug_envsphere_shm.reset(new sq::Shader(gl::FRAGMENT_SHADER));
    FS.dbug_envsphere_sdv.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VS.gbuf_base_stencil->add_uniform("matrix"); // mat4
    VS.gbuf_refl_stencil->add_uniform("matrix"); // mat4
    VS.shad_simple->add_uniform("matrix"); // mat4
    VS.shad_skelly->add_uniform("matrix"); // mat4
    //VS.irrd_gbuf_simple->add_uniform("matrix"); // mat4
    //VS.irrd_gbuf_simple->add_uniform("normMat"); // mat3
    //VS.irrd_defr_skybox->add_uniform("faceInd"); // uint
    VS.dbug_envsphere->add_uniform("trans"); // vec3
    FS.gbuf_base_model->add_uniform("d_n_s"); // ivec3
    FS.gbuf_refl_model->add_uniform("d_n_s"); // ivec3
    //FS.irrd_gbuf_model->add_uniform("d_n_s"); // ivec3
    //FS.irrd_defr_skylight->add_uniform("faceInd"); // uint
    //FS.irrd_defr_spotlight->add_uniform("faceInd"); // uint
    //FS.irrd_defr_pointlight->add_uniform("faceInd"); // uint
    FS.dbug_envsphere_shm->add_uniform("probe"); // uint
    FS.dbug_envsphere_sdv->add_uniform("probe"); // uint

    /// Load Vertex Shaders
    VS.gnrc_screen->load(app->preprocs.load("generic/screen_vs"));
    VS.gbuf_base_stencil->load(app->preprocs.load("gbuffer/base/stencil_vs"));
    VS.gbuf_base_simple->load(app->preprocs.load("gbuffer/base/simple_vs"));
    VS.gbuf_base_skelly->load(app->preprocs.load("gbuffer/base/skelly_vs"));
    VS.gbuf_base_decal->load(app->preprocs.load("gbuffer/base/decal_vs"));
    VS.gbuf_refl_stencil->load(app->preprocs.load("gbuffer/refl/stencil_vs"));
    VS.gbuf_refl_simple->load(app->preprocs.load("gbuffer/refl/simple_vs"));
    VS.gbuf_refl_skelly->load(app->preprocs.load("gbuffer/refl/skelly_vs"));
    VS.gbuf_refl_decal->load(app->preprocs.load("gbuffer/refl/decal_vs"));
    VS.shad_simple->load(app->preprocs.load("shadows/simple_vs"));
    VS.shad_skelly->load(app->preprocs.load("shadows/skelly_vs"));
    VS.irrd_gbuf_simple->load(app->preprocs.load("irradiance/gbuf/simple_vs"));
    VS.irrd_defr_skybox->load(app->preprocs.load("irradiance/defr/skybox_vs"));
    VS.defr_reflector->load(app->preprocs.load("deferred/reflector_vs"));
    VS.defr_base_skybox->load(app->preprocs.load("deferred/base/skybox_vs"));
    VS.defr_refl_skybox->load(app->preprocs.load("deferred/refl/skybox_vs"));
    VS.part_soft_vertex->load(app->preprocs.load("particles/soft/vertex_vs"));
    VS.prty_shafts_shafts->load(app->preprocs.load("pretty/shafts/shafts_vs"));
    VS.dbug_envsphere->load(app->preprocs.load("debug/envsphere_vs"));

    /// Load Geometry Shaders
    GS.part_soft_geometry->load(app->preprocs.load("particles/soft/geometry_gs"));

    /// Load Fragment Shaders
    FS.gnrc_lumalpha->load(app->preprocs.load("generic/lumalpha_fs"));
    FS.gnrc_passthru->load(app->preprocs.load("generic/passthru_fs"));
    FS.gbuf_base_model->load(app->preprocs.load("gbuffer/base/model_fs"));
    FS.gbuf_base_decal->load(app->preprocs.load("gbuffer/base/decal_fs"));
    FS.gbuf_refl_model->load(app->preprocs.load("gbuffer/refl/model_fs"));
    FS.gbuf_refl_decal->load(app->preprocs.load("gbuffer/refl/decal_fs"));
    FS.shad_punch->load(app->preprocs.load("shadows/punch_fs"));
    FS.irrd_gbuf_model->load(app->preprocs.load("irradiance/gbuf/model_fs"));
    FS.irrd_defr_skybox->load(app->preprocs.load("irradiance/defr/skybox_fs"));
    FS.irrd_defr_skylight->load(app->preprocs.load("irradiance/defr/skylight_fs"));
    FS.irrd_defr_spotlight->load(app->preprocs.load("irradiance/defr/spotlight_fs"));
    FS.irrd_defr_pointlight->load(app->preprocs.load("irradiance/defr/pointlight_fs"));
    FS.irrd_harmonics_shm->load(app->preprocs.load("irradiance/harmonics_fs"));
    FS.irrd_harmonics_sdv->load(app->preprocs.load("irradiance/harmonics_fs", "#define SUBDIV"));
    FS.defr_skybox->load(app->preprocs.load("deferred/skybox_fs"));
    FS.defr_reflector->load(app->preprocs.load("deferred/reflector_fs"));
    FS.defr_refl_ambient->load(app->preprocs.load("deferred/refl/ambient_fs"));
    FS.part_soft_ambient->load(app->preprocs.load("particles/soft/ambient_fs"));
    FS.part_soft_write->load(app->preprocs.load("particles/soft/write_fs"));
    FS.prty_shafts_write->load(app->preprocs.load("pretty/shafts/write_fs"));
    FS.prty_hdr_highs->load(app->preprocs.load("pretty/hdr/highs_fs"));
    FS.prty_vignette->load(app->preprocs.load("pretty/vignette_fs"));
    FS.dbug_envsphere_shm->load(app->preprocs.load("debug/envsphere_fs"));
    FS.dbug_envsphere_sdv->load(app->preprocs.load("debug/envsphere_fs", "#define SUBDIV"));

    /// Set Graph Vertex Shaders
    graph->VS.gnrc_screen = VS.gnrc_screen.get();
    graph->VS.gbuf_base_stencil = VS.gbuf_base_stencil.get();
    graph->VS.gbuf_base_simple = VS.gbuf_base_simple.get();
    graph->VS.gbuf_base_skelly = VS.gbuf_base_skelly.get();
    graph->VS.gbuf_base_decal = VS.gbuf_base_decal.get();
    graph->VS.gbuf_refl_stencil = VS.gbuf_refl_stencil.get();
    graph->VS.gbuf_refl_simple = VS.gbuf_refl_simple.get();
    graph->VS.gbuf_refl_skelly = VS.gbuf_refl_skelly.get();
    graph->VS.gbuf_refl_decal = VS.gbuf_refl_decal.get();
    graph->VS.shad_simple = VS.shad_simple.get();
    graph->VS.shad_skelly = VS.shad_skelly.get();
    graph->VS.irrd_gbuf_simple = VS.irrd_gbuf_simple.get();
    graph->VS.irrd_defr_skybox = VS.irrd_defr_skybox.get();
    graph->VS.defr_reflector = VS.defr_reflector.get();
    graph->VS.defr_base_skybox = VS.defr_base_skybox.get();
    graph->VS.defr_refl_skybox = VS.defr_refl_skybox.get();
    graph->VS.part_soft_vertex = VS.part_soft_vertex.get();
    graph->VS.dbug_envsphere = VS.dbug_envsphere.get();

    /// Set Graph Geometry Shaders
    graph->GS.part_soft_geometry = GS.part_soft_geometry.get();

    /// Set Graph Fragment Shaders
    graph->FS.gnrc_lumalpha = FS.gnrc_lumalpha.get();
    graph->FS.gnrc_passthru = FS.gnrc_passthru.get();
    graph->FS.gbuf_base_model = FS.gbuf_base_model.get();
    graph->FS.gbuf_base_decal = FS.gbuf_base_decal.get();
    graph->FS.gbuf_refl_model = FS.gbuf_refl_model.get();
    graph->FS.gbuf_refl_decal = FS.gbuf_refl_decal.get();
    graph->FS.shad_punch = FS.shad_punch.get();
    graph->FS.irrd_gbuf_model = FS.irrd_gbuf_model.get();
    graph->FS.irrd_defr_skybox = FS.irrd_defr_skybox.get();
    graph->FS.irrd_defr_skylight = FS.irrd_defr_skylight.get();
    graph->FS.irrd_defr_spotlight = FS.irrd_defr_spotlight.get();
    graph->FS.irrd_defr_pointlight = FS.irrd_defr_pointlight.get();
    graph->FS.irrd_harmonics_shm = FS.irrd_harmonics_shm.get();
    graph->FS.irrd_harmonics_sdv = FS.irrd_harmonics_sdv.get();
    graph->FS.defr_skybox = FS.defr_skybox.get();
    graph->FS.defr_base_ambient = FS.defr_base_ambient.get();
    graph->FS.defr_base_skylight = FS.defr_base_skylight.get();
    graph->FS.defr_base_spot_none = FS.defr_base_spot_none.get();
    graph->FS.defr_base_spot_shad = FS.defr_base_spot_shad.get();
    graph->FS.defr_base_spot_spec = FS.defr_base_spot_spec.get();
    graph->FS.defr_base_spot_both = FS.defr_base_spot_both.get();
    graph->FS.defr_base_point_none = FS.defr_base_point_none.get();
    graph->FS.defr_base_point_shad = FS.defr_base_point_shad.get();
    graph->FS.defr_base_point_spec = FS.defr_base_point_spec.get();
    graph->FS.defr_base_point_both = FS.defr_base_point_both.get();
    graph->FS.defr_refl_ambient = FS.defr_refl_ambient.get();
    graph->FS.defr_refl_skylight = FS.defr_refl_skylight.get();
    graph->FS.defr_refl_spot_none = FS.defr_refl_spot_none.get();
    graph->FS.defr_refl_spot_shad = FS.defr_refl_spot_shad.get();
    graph->FS.defr_refl_point_none = FS.defr_refl_point_none.get();
    graph->FS.defr_refl_point_shad = FS.defr_refl_point_shad.get();
    graph->FS.defr_reflector = FS.defr_reflector.get();
    graph->FS.part_soft_ambient = FS.part_soft_ambient.get();
    graph->FS.part_soft_skylight = FS.part_soft_skylight.get();
    graph->FS.part_soft_spot_none = FS.part_soft_spot_none.get();
    graph->FS.part_soft_spot_shad = FS.part_soft_spot_shad.get();
    graph->FS.part_soft_point_none = FS.part_soft_point_none.get();
    graph->FS.part_soft_point_shad = FS.part_soft_point_shad.get();
    graph->FS.part_soft_write = FS.part_soft_write.get();
    graph->FS.dbug_envsphere_shm = FS.dbug_envsphere_shm.get();
    graph->FS.dbug_envsphere_sdv = FS.dbug_envsphere_sdv.get();

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

    if (app->console.active == false) {
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

void MainScene::render() {

    camera->pos = glm::mix(posCrnt, posNext, accum * 24.f);
    if (app->console.active == false) {
        fvec2 mMove = app->mouse_centre();
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
    gl::UseProgram(0u);
    sq::DEPTH_ON(); sq::DEPTH_WRITE();
    sq::BLEND_OFF(); sq::STENCIL_OFF();
    sq::CULLFACE_ON(); sq::CULLFACE_BACK();
    graph->render_shadows_sky_main();
    graph->render_shadows_sky_box();
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
    graph->render_msimples_base(true);
    graph->render_mskellys_base(true);
    graph->render_reflects_base(true);
    graph->render_decals_base();
    graph->render_msimples_base(false);
    graph->render_mskellys_base(false);
    graph->render_reflects_base(false);


    /// Render Downscaled Depth Buffers
    FB.defrBase->bind(gl::READ_FRAMEBUFFER); FB.depHalf->bind(gl::DRAW_FRAMEBUFFER);
    sq::BLIT_FRAMEBUFFER(INFO.fullSize, INFO.halfSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);
    FB.defrBase->bind(gl::READ_FRAMEBUFFER); FB.depQter->bind(gl::DRAW_FRAMEBUFFER);
    sq::BLIT_FRAMEBUFFER(INFO.fullSize, INFO.qterSize, gl::DEPTH_BUFFER_BIT, gl::NEAREST);


    /// Render SSAO Texture
    if (INFO.ssao != 0) {
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


    /// Render Lights and Reflections
    graph->render_skybox_base();
    graph->render_ambient_base();
    graph->render_skylight_base();
    graph->render_spotlights_base();
    graph->render_pointlights_base();
    graph->render_reflections();


    /// Render Light Shafts Texture
    if (INFO.shafts != 0) {
        world->skylight.ubo->bind(1u);
        sq::DEPTH_OFF(); sq::BLEND_OFF();
        sq::STENCIL_OFF();

        sq::VIEWPORT(INFO.qterSize);
        FB.shafts->use(); sq::CLEAR_COLOR();
        pipeline->use_shader(*VS.prty_shafts_shafts);
        pipeline->use_shader(*FS.prty_shafts_shafts);
        world->skylight.texDepthA->bind(gl::TEXTURE0);
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


    /// Render Particles
    graph->render_particles();


    /// Write HDR Luma to Alpha
    sq::DEPTH_OFF(); sq::STENCIL_OFF();
    sq::BLEND_OFF(); sq::CULLFACE_OFF();
    pipeline->use_shader(*VS.gnrc_screen);
    pipeline->use_shader(*FS.gnrc_lumalpha);
    FB.hdrBase->use(); sq::VIEWPORT(INFO.fullSize);
    TX.hdrBase->bind(gl::TEXTURE0);
    sq::draw_screen_quad();


    /// Render Bloom Texture
    if (INFO.bloom == true) {
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
    if (INFO.fxaa != 0) {
        pipeline->use_shader(*FS.gnrc_lumalpha);
        sq::draw_screen_quad();
        pipeline->use_shader(*FS.prty_fxaa_fxaa);
    } else pipeline->use_shader(*FS.gnrc_passthru);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    sq::draw_screen_quad();


    /// Vignetting Effect
    if (INFO.vignette == true) {
        pipeline->use_shader(*FS.prty_vignette);
        sq::BLEND_ON(); sq::BLEND_ALPHA();
        sq::draw_screen_quad();
    }

//    graph->irrVolume.texHdr->bind(gl::TEXTURE0);
//    sq::debug_draw_texture2DArray(0);

//    graph->irrVolume.texSHrm->bind(gl::TEXTURE0);
//    sq::debug_draw_texture2D();

//    graph->render_envspheres();

    gl::BindProgramPipeline(0);
    gl::BindVertexArray(0);
}


void MainScene::refresh() {
    INFO.fullSize = app->get_size();
    INFO.halfSize = INFO.fullSize / 2u;
    INFO.qterSize = INFO.fullSize / 4u;
    INFO.fPixSize = 1.f / fvec2(INFO.fullSize);
    INFO.hPixSize = 1.f / fvec2(INFO.halfSize);
    INFO.qPixSize = 1.f / fvec2(INFO.qterSize);
    INFO.aspect = float(INFO.fullSize.x) /
                  float(INFO.fullSize.y);

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

    INFO.viewdist   = app->settings.get<float>("rpg_viewdist");
    INFO.shadfilter = app->settings.get<bool>("rpg_shadfilter");
    INFO.shadlarge  = app->settings.get<bool>("rpg_shadlarge");
    INFO.vignette   = app->settings.get<bool>("rpg_vignette");
    INFO.bloom      = app->settings.get<bool>("rpg_bloom");
    INFO.irrd       = app->settings.get<bool>("rpg_irrd");
    INFO.shafts     = app->settings.get<int>("rpg_shafts");
    INFO.fxaa       = app->settings.get<int>("rpg_fxaa");
    INFO.ssao       = app->settings.get<int>("rpg_ssao");

    graph->INFO.viewdist = INFO.viewdist;
    graph->INFO.shadlarge = INFO.shadlarge;
    graph->INFO.shadfilter = INFO.shadfilter;
    graph->INFO.irrd = INFO.irrd;
    camera->rmax = INFO.viewdist;

    world->invalidate();
    world->refresh();
    graph->update_settings();
    reload_shaders();
}


void MainScene::reload_shaders() {
    const sq::PreProcessor& pp = app->preprocs;

    string defLightBase = "";
    if (INFO.shadlarge) defLightBase += "#define LARGE\n";
    if (INFO.shadfilter) defLightBase += "#define FILTER";
    string defLightShad = defLightBase + "\n#define SHADOW";
    string defLightSpec = defLightBase + "\n#define SPECULAR";
    string defLightBoth = defLightShad + "\n#define SPECULAR";

    string defHigh = "#define HIGH";
    string defAspect = "\n#define ASPECT " + std::to_string(INFO.aspect);
    string defFPixSize = "\n#define PIXSIZE " + glm::to_string(INFO.fPixSize);
    string defHPixSize = "\n#define PIXSIZE " + glm::to_string(INFO.hPixSize);
    string defQPixSize = "\n#define PIXSIZE " + glm::to_string(INFO.qPixSize);

    /// Lighting
    FS.defr_base_skylight->load(pp.load("deferred/base/skylight_fs", defLightBase));
    FS.defr_base_spot_none->load(pp.load("deferred/base/spotlight_fs", defLightBase));
    FS.defr_base_spot_shad->load(pp.load("deferred/base/spotlight_fs", defLightShad));
    FS.defr_base_spot_spec->load(pp.load("deferred/base/spotlight_fs", defLightSpec));
    FS.defr_base_spot_both->load(pp.load("deferred/base/spotlight_fs", defLightBoth));
    FS.defr_base_point_none->load(pp.load("deferred/base/pointlight_fs", defLightBase));
    FS.defr_base_point_shad->load(pp.load("deferred/base/pointlight_fs", defLightShad));
    FS.defr_base_point_spec->load(pp.load("deferred/base/pointlight_fs", defLightSpec));
    FS.defr_base_point_both->load(pp.load("deferred/base/pointlight_fs", defLightBoth));
    FS.defr_refl_skylight->load(pp.load("deferred/refl/skylight_fs", defLightBase));
    FS.defr_refl_spot_none->load(pp.load("deferred/refl/spotlight_fs", defLightBase));
    FS.defr_refl_spot_shad->load(pp.load("deferred/refl/spotlight_fs", defLightShad));
    FS.defr_refl_point_none->load(pp.load("deferred/refl/pointlight_fs", defLightBase));
    FS.defr_refl_point_shad->load(pp.load("deferred/refl/pointlight_fs", defLightShad));
    FS.part_soft_skylight->load(pp.load("particles/soft/skylight_fs", defLightBase));
    FS.part_soft_spot_none->load(pp.load("particles/soft/spotlight_fs", defLightBase));
    FS.part_soft_spot_shad->load(pp.load("particles/soft/spotlight_fs", defLightShad));
    FS.part_soft_point_none->load(pp.load("particles/soft/pointlight_fs", defLightBase));
    FS.part_soft_point_shad->load(pp.load("particles/soft/pointlight_fs", defLightShad));

    /// SSAO
    if (INFO.ssao==0) FS.defr_base_ambient->load(pp.load("deferred/base/ambient_fs"));
    else { FS.prty_ssao_blur->load(pp.load("pretty/ssao/blur_fs", defHPixSize));
        if (INFO.ssao==1) FS.prty_ssao_ssao->load(pp.load("pretty/ssao/ssao_fs", defAspect));
        else FS.prty_ssao_ssao->load(pp.load("pretty/ssao/ssao_fs", defHigh + defAspect));
        FS.defr_base_ambient->load(pp.load("deferred/base/ambient_fs", "#define SSAO")); }

    /// Shafts
    if (INFO.shafts==1) FS.prty_shafts_shafts->load(pp.load("pretty/shafts/shafts_fs"));
    if (INFO.shafts==2) FS.prty_shafts_shafts->load(pp.load("pretty/shafts/shafts_fs", defHigh));

    /// Bloom
    if (INFO.bloom==true) FS.prty_hdr_blurv->load(pp.load("pretty/hdr/blurv_fs")),
                          FS.prty_hdr_blurh->load(pp.load("pretty/hdr/blurh_fs", defAspect));

    /// HDR
    if (INFO.bloom==false) FS.prty_hdr_tones->load(pp.load("pretty/hdr/tones_fs"));
    else FS.prty_hdr_tones->load(pp.load("pretty/hdr/tones_fs", "#define BLOOM"));

    /// FXAA
    if (INFO.fxaa==1) FS.prty_fxaa_fxaa->load(pp.load("pretty/fxaa/fxaa_fs", defFPixSize));
    if (INFO.fxaa==2) FS.prty_fxaa_fxaa->load(pp.load("pretty/fxaa/fxaa_fs", defHigh + defFPixSize));
}
