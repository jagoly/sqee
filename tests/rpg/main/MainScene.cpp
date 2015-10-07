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

#include <sqee/debug/OpenGL.hpp>

#include "../rndr/Shadows.hpp"
#include "../rndr/Gbuffers.hpp"
#include "../rndr/Lighting.hpp"
#include "../rndr/Pretties.hpp"
#include "../rndr/Renderer.hpp"
#include "../wcoe/World.hpp"
#include "../wcoe/Cell.hpp"
#include "MainScene.hpp"

#include <sqee/debug/Misc.hpp>

using namespace sqt;

MainScene::MainScene(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 24u;

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

    camera.reset(new sq::Camera());
    pipeline.reset(new sq::Pipeline());
    world.reset(new wcoe::World(app->settings, camera.get()));
    renderer.reset(new rndr::Renderer(app->settings, app->preprocs, *pipeline, *world));

    renderer->camera = camera.get();
    renderer->skylight = &world->skylight;

    app->cs->add_global(chai::var(camera.get()), "camera");
    app->cs->add_global(chai::var(world.get()), "world");
    app->cs->add_global(chai::var(renderer.get()), "renderer");

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
    app->settings.add<int>("rpg_fsaa", 2);


    /// Create Textures
    TX.pshadA.reset(new sq::TextureMut2D());
    TX.pshadB.reset(new sq::TextureMut2D());
    TX.reflDiff.reset(new sq::TextureMut2D());
    TX.reflSurf.reset(new sq::TextureMut2D());
    TX.reflDpSt.reset(new sq::TextureMut2D());
    TX.partMain.reset(new sq::TextureMut2D());
    TX.partDpSt.reset(new sq::TextureMut2D());
    TX.hdrRefl.reset(new sq::TextureMut2D());
    TX.hdrPart.reset(new sq::TextureMut2D());

    /// Setup Textures
    TX.pshadA->create(gl::RED, gl::R8, 1u);
    TX.pshadB->create(gl::RED, gl::R8, 1u);
    TX.reflDiff->create(gl::RGB, gl::RGB8, 3u);
    TX.reflSurf->create(gl::RGB, gl::RGB8, 3u);
    TX.reflDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u);
    TX.partMain->create(gl::RGBA, gl::RGBA16F, 4u);
    TX.partDpSt->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1u);
    TX.hdrRefl->create(gl::RGB, gl::RGB16F, 3u);
    TX.hdrPart->create(gl::RGBA, gl::RGBA16F, 4u);

    /// Set Texture Presets
    TX.pshadA->set_preset(sq::Texture::LinearClamp());
    TX.pshadB->set_preset(sq::Texture::LinearClamp());
    TX.reflDiff->set_preset(sq::Texture::LinearClamp());
    TX.reflSurf->set_preset(sq::Texture::LinearClamp());
    TX.reflDpSt->set_preset(sq::Texture::LinearClamp());
    TX.partMain->set_preset(sq::Texture::LinearClamp());
    TX.partDpSt->set_preset(sq::Texture::LinearClamp());
    TX.hdrRefl->set_preset(sq::Texture::LinearClamp());
    TX.hdrPart->set_preset(sq::Texture::LinearClamp());

    /// Create Framebuffers
    FB.pshadA.reset(new sq::FrameBuffer());
    FB.pshadB.reset(new sq::FrameBuffer());
    FB.defrRefl.reset(new sq::FrameBuffer());
    FB.defrPart.reset(new sq::FrameBuffer());
    FB.hdrRefl.reset(new sq::FrameBuffer());
    FB.hdrPart.reset(new sq::FrameBuffer());

    /// Setup Framebuffers
    FB.pshadA->attach(gl::COLOR_ATTACHMENT0, *TX.pshadA);
    FB.pshadB->attach(gl::COLOR_ATTACHMENT0, *TX.pshadB);
    FB.defrRefl->attach(gl::COLOR_ATTACHMENT0, *TX.reflDiff);
    FB.defrRefl->attach(gl::COLOR_ATTACHMENT1, *TX.reflSurf);
    FB.defrRefl->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.reflDpSt);
    FB.defrPart->attach(gl::COLOR_ATTACHMENT0, *TX.partMain);
    FB.defrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);
    FB.hdrRefl->attach(gl::COLOR_ATTACHMENT0, *TX.hdrRefl);
    FB.hdrRefl->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.reflDpSt);
    FB.hdrPart->attach(gl::COLOR_ATTACHMENT0, *TX.hdrPart);
    FB.hdrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);

    FB.pshadA->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.pshadB->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.defrRefl->draw_buffers({gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1});
    FB.defrPart->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.hdrRefl->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.hdrPart->draw_buffers({gl::COLOR_ATTACHMENT0});


    /// Set Graph Textures
    renderer->TX.pshadA = TX.pshadA.get();
    renderer->TX.pshadB = TX.pshadB.get();
    renderer->TX.reflDiff = TX.reflDiff.get();
    renderer->TX.reflSurf = TX.reflSurf.get();
    renderer->TX.reflDpSt = TX.reflDpSt.get();
    renderer->TX.partMain = TX.partMain.get();
    renderer->TX.partDpSt = TX.partDpSt.get();
    renderer->TX.hdrRefl = TX.hdrRefl.get();
    renderer->TX.hdrPart = TX.hdrPart.get();

    /// Set Graph Framebuffers
    renderer->FB.pshadA = FB.pshadA.get();
    renderer->FB.pshadB = FB.pshadB.get();
    renderer->FB.defrRefl = FB.defrRefl.get();
    renderer->FB.defrPart = FB.defrPart.get();
    renderer->FB.hdrRefl = FB.hdrRefl.get();
    renderer->FB.hdrPart = FB.hdrPart.get();
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


    renderer->cull_and_sort();


    /// Setup Stuff
    pipeline->bind();
    camera->ubo->bind(0u);
    gl::DepthFunc(gl::LEQUAL);


    /// Light Shadows
    renderer->shadows->setup_render_state();
    renderer->shadows->render_shadows_sky();
    renderer->shadows->render_shadows_spot();
    renderer->shadows->render_shadows_point();


    /// Render Models into G-Buffer
    renderer->gbuffers->setup_render_state();
    renderer->gbuffers->render_msimples_base(true);
    renderer->gbuffers->render_mskellys_base(true);
    renderer->gbuffers->render_reflects_base(true);
    renderer->gbuffers->bind_textures_base();
    renderer->gbuffers->render_decals_base();
    renderer->gbuffers->render_msimples_base(false);
    renderer->gbuffers->render_mskellys_base(false);
    renderer->gbuffers->render_reflects_base(false);


    /// Render Downscaled Depth Buffers
    renderer->gbuffers->render_downscaled_depth();


    /// Render SSAO Texture
    if (INFO.ssao != 0) {
        renderer->pretties->setup_render_state();
        renderer->pretties->render_ssao_texture();
    }


    /// Clear HDR Framebuffer


    /// Render Lights and Reflections
    renderer->lighting->setup_render_state();
    renderer->lighting->render_skybox_base();
    renderer->lighting->render_ambient_base();
    renderer->lighting->render_skylight_base();
    renderer->lighting->render_spotlights_base();
    renderer->lighting->render_pointlights_base();
    //renderer->render_reflections();


    /// Render Light Shafts Texture


    /// Render Particles
    renderer->render_particles();

    /// Render Bloom Texture
    renderer->pretties->setup_render_state();
    if (INFO.shafts) renderer->pretties->render_shafts_texture();
    if (INFO.bloom) renderer->pretties->render_bloom_texture();
    renderer->pretties->render_hdr_tonemap();


    /// Antialiasing and Write to Screen
    renderer->pretties->render_fsaa_screen();


    /// Vignetting Effect
    renderer->pretties->render_vignette();


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

    renderer->INFO.fullSize = INFO.fullSize;
    renderer->INFO.halfSize = INFO.halfSize;
    renderer->INFO.qterSize = INFO.qterSize;
    renderer->INFO.fPixSize = INFO.fPixSize;
    renderer->INFO.hPixSize = INFO.hPixSize;
    renderer->INFO.qPixSize = INFO.qPixSize;

    TX.pshadA->resize(INFO.halfSize);
    TX.pshadB->resize(INFO.halfSize);
    TX.reflDiff->resize(INFO.halfSize);
    TX.reflSurf->resize(INFO.halfSize);
    TX.reflDpSt->resize(INFO.halfSize);
    TX.partMain->resize(INFO.halfSize);
    TX.partDpSt->resize(INFO.halfSize);
    TX.hdrRefl->resize(INFO.halfSize);
    TX.hdrPart->resize(INFO.halfSize);

    camera->size = fvec2(INFO.fullSize);

    INFO.viewdist   = app->settings.get<float>("rpg_viewdist");
    INFO.shadfilter = app->settings.get<bool>("rpg_shadfilter");
    INFO.shadlarge  = app->settings.get<bool>("rpg_shadlarge");
    INFO.vignette   = app->settings.get<bool>("rpg_vignette");
    INFO.bloom      = app->settings.get<bool>("rpg_bloom");
    INFO.shafts     = app->settings.get<int>("rpg_shafts");
    INFO.ssao       = app->settings.get<int>("rpg_ssao");
    INFO.fsaa       = app->settings.get<int>("rpg_fsaa");

    renderer->INFO.viewdist = INFO.viewdist;
    renderer->INFO.shadlarge = INFO.shadlarge;
    renderer->INFO.shadfilter = INFO.shadfilter;
    renderer->INFO.aspect = INFO.aspect;
    renderer->INFO.bloom = INFO.bloom;
    renderer->INFO.shafts = INFO.shafts;
    renderer->INFO.ssao = INFO.ssao;
    renderer->INFO.fsaa = INFO.fsaa;
    camera->rmax = INFO.viewdist;

    world->invalidate();
    world->refresh();
    renderer->update_settings();
    reload_shaders();
}


void MainScene::reload_shaders() {
}
