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
#include "../rndr/Reflects.hpp"
#include "../rndr/Renderer.hpp"
#include "../wcoe/World.hpp"
#include "../wcoe/Cell.hpp"
#include "MainScene.hpp"

using namespace sqt;
namespace maths = sq::maths;

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
    TX.pshadA.reset(new sq::Texture2D(gl::RED, gl::R8, sq::Texture::LinearClamp()));
    TX.pshadB.reset(new sq::Texture2D(gl::RED, gl::R8, sq::Texture::LinearClamp()));
    TX.partMain.reset(new sq::Texture2D(gl::RGBA, gl::RGBA16F, sq::Texture::LinearClamp()));
    TX.partDpSt.reset(new sq::Texture2D(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::LinearClamp()));
    TX.hdrPart.reset(new sq::Texture2D(gl::RGBA, gl::RGBA16F, sq::Texture::LinearClamp()));

    /// Create Framebuffers
    FB.pshadA.reset(new sq::FrameBuffer());
    FB.pshadB.reset(new sq::FrameBuffer());
    FB.defrPart.reset(new sq::FrameBuffer());
    FB.hdrPart.reset(new sq::FrameBuffer());

    /// Setup Framebuffers
    FB.pshadA->attach(gl::COLOR_ATTACHMENT0, *TX.pshadA);
    FB.pshadB->attach(gl::COLOR_ATTACHMENT0, *TX.pshadB);
    FB.defrPart->attach(gl::COLOR_ATTACHMENT0, *TX.partMain);
    FB.defrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);
    FB.hdrPart->attach(gl::COLOR_ATTACHMENT0, *TX.hdrPart);
    FB.hdrPart->attach(gl::DEPTH_STENCIL_ATTACHMENT, *TX.partDpSt);

    FB.pshadA->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.pshadB->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.defrPart->draw_buffers({gl::COLOR_ATTACHMENT0});
    FB.hdrPart->draw_buffers({gl::COLOR_ATTACHMENT0});


    /// Set Graph Textures
    renderer->TX.pshadA = TX.pshadA.get();
    renderer->TX.pshadB = TX.pshadB.get();
    renderer->TX.partMain = TX.partMain.get();
    renderer->TX.partDpSt = TX.partDpSt.get();
    renderer->TX.hdrPart = TX.hdrPart.get();

    /// Set Graph Framebuffers
    renderer->FB.pshadA = FB.pshadA.get();
    renderer->FB.pshadB = FB.pshadB.get();
    renderer->FB.defrPart = FB.defrPart.get();
    renderer->FB.hdrPart = FB.hdrPart.get();
}



void MainScene::update() {
    using KB = sf::Keyboard;
    posCrnt = posNext;

    if (app->console.active == false) {
        if (KB::isKeyPressed(KB::PageUp)) posNext.z += 0.05f;
        if (KB::isKeyPressed(KB::PageDown)) posNext.z -= 0.05f;

        if (KB::isKeyPressed(KB::Right) && !KB::isKeyPressed(KB::Left))
            posNext += maths::rotate_z(Vec3F(+0.08f, 0.f, 0.f), rotZ);
        else if (KB::isKeyPressed(KB::Left) && !KB::isKeyPressed(KB::Right))
            posNext += maths::rotate_z(Vec3F(-0.08f, 0.f, 0.f), rotZ);

        if (KB::isKeyPressed(KB::Up) && !KB::isKeyPressed(KB::Down))
            posNext += maths::rotate_z(Vec3F(0.f, +0.08f, 0.f), rotZ);
        else if (KB::isKeyPressed(KB::Down) && !KB::isKeyPressed(KB::Up))
            posNext += maths::rotate_z(Vec3F(0.f, -0.08f, 0.f), rotZ);
    }

    world->update();
}

void MainScene::render() {
    if (app->console.active == false) {
        Vec2F mMove = app->mouse_centre();
        rotZ = rotZ + mMove.x / 600.f;
        rotX = maths::clamp(rotX + mMove.y / 900.f, -1.25f, 1.25f);
        camera->dir = maths::rotate_z(maths::rotate_x(Vec3F(0.f, 1.f, 0.f), rotX), rotZ);
        camera->pos = maths::mix(posCrnt, posNext, float(accum)*24.f);
    }

    camera->update();
    world->calc(accum);


    renderer->cull_and_sort();


    /// Setup Stuff
    pipeline->bind();
    camera->ubo.bind(0u);
    gl::DepthFunc(gl::LEQUAL);

    renderer->shadows->setup_render_state();
    renderer->shadows->render_shadows_sky();
    renderer->shadows->render_shadows_spot();
    renderer->shadows->render_shadows_point();

    renderer->gbuffers->render_gbuffers_base();

    renderer->pretties->render_post_gbuffers();

    renderer->lighting->render_lighting_base();

    renderer->reflects->render_reflections();

    //renderer->render_particles();

    renderer->pretties->render_post_lighting();

    renderer->pretties->render_final_screen();


    gl::BindProgramPipeline(0);
    gl::BindVertexArray(0);
}


void MainScene::refresh() {
//    TX.pshadA->allocate_storage(INFO.halfSize, false);
//    TX.pshadB->allocate_storage(INFO.halfSize, false);
//    TX.partMain->allocate_storage(INFO.halfSize, false);
//    TX.partDpSt->allocate_storage(INFO.halfSize, false);
//    TX.hdrPart->allocate_storage(INFO.halfSize, false);

    camera->size = Vec2F(app->settings.get<int>("app_width"),
                         app->settings.get<int>("app_height"));

    INFO.vignette   = app->settings.get<bool>("rpg_vignette");
    INFO.bloom      = app->settings.get<bool>("rpg_bloom");
    INFO.shafts     = app->settings.get<int>("rpg_shafts");
    INFO.ssao       = app->settings.get<int>("rpg_ssao");
    INFO.fsaa       = app->settings.get<int>("rpg_fsaa");

    world->invalidate();
    world->refresh();
    renderer->update_settings();
    reload_shaders();
}


void MainScene::reload_shaders() {
}
