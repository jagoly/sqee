#include <rp3d/engine/DynamicsWorld.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>

#include <sqee/debug/Misc.hpp>

#include "../rndr/Shadows.hpp"
#include "../rndr/Gbuffers.hpp"
#include "../rndr/Lighting.hpp"
#include "../rndr/Pretties.hpp"
#include "../rndr/Reflects.hpp"
#include "../rndr/Renderer.hpp"
#include "../wcoe/Camera.hpp"
#include "../wcoe/World.hpp"
#include "Scripting.hpp"
#include "MainScene.hpp"


using namespace sqt;
namespace maths = sq::maths;

MainScene::MainScene(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 24u;

    /// Import GLSL Headers
    app->preprocs.import_header("headers/blocks/msimple");
    app->preprocs.import_header("headers/blocks/mskelly");
    app->preprocs.import_header("headers/blocks/ambient");
    app->preprocs.import_header("headers/blocks/skylight");
    app->preprocs.import_header("headers/blocks/spotlight");
    app->preprocs.import_header("headers/blocks/pointlight");
    app->preprocs.import_header("headers/blocks/reflect");
    app->preprocs.import_header("headers/blocks/skybox");
    app->preprocs.import_header("headers/blocks/decal");
    app->preprocs.import_header("headers/shadow/sample_sky");
    app->preprocs.import_header("headers/shadow/sample_spot");
    app->preprocs.import_header("headers/shadow/sample_point");

    pipeline.reset(new sq::Pipeline());
    world.reset(new World(app->messageBus, app->settings));
    renderer.reset(new Renderer(app->messageBus, app->settings, app->preprocs, *pipeline, *world));

    posCrnt = posNext = world->camera->PROP_position;

    cs_setup_components(*app->cs, *world);
    cs_setup_functions(*app->cs, *world);

    app->cs->add_global(chai::var(world.get()), "world");
    app->cs->add_global(chai::var(renderer.get()), "renderer");


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

/*
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
*/

    // /////////////////////////////////////////////// //

    //app->messageBus.send_message("TransformComponentUpdated", eDiceGroup);
    //app->messageBus.send_message("ModelComponentUpdated", eDiceGroup);
}



void MainScene::tick() {
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

    world->tick();
}

void MainScene::render() {
    float tickPercent = accum * 24.0;
    world->tickPercent = tickPercent;
    renderer->tickPercent = tickPercent;

    if (app->console.active == false) {
        Vec2F mMove = app->mouse_centre();
        rotZ = rotZ + mMove.x / 1600.f;
        rotX = maths::clamp(rotX + mMove.y / 2400.f, -0.23f, 0.23f);
        world->camera->PROP_direction = maths::rotate_z(maths::rotate_x(Vec3F(0.f, 1.f, 0.f), rotX), rotZ);
        world->camera->PROP_position = maths::mix(posCrnt, posNext, tickPercent);
    }

    world->update();

    // Setup Stuff /////

    pipeline->bind();

    renderer->render_scene();

    gl::BindProgramPipeline(0u);
    gl::BindVertexArray(0u);
}


void MainScene::configure() {
//    TX.pshadA->allocate_storage(INFO.halfSize, false);
//    TX.pshadB->allocate_storage(INFO.halfSize, false);
//    TX.partMain->allocate_storage(INFO.halfSize, false);
//    TX.partDpSt->allocate_storage(INFO.halfSize, false);
//    TX.hdrPart->allocate_storage(INFO.halfSize, false);

    world->camera->size = Vec2F(app->settings.get<int>("app_width"),
                                app->settings.get<int>("app_height"));

    INFO.vignette   = app->settings.get<bool>("rpg_vignette");
    INFO.bloom      = app->settings.get<bool>("rpg_bloom");
    INFO.shafts     = app->settings.get<int>("rpg_shafts");
    INFO.ssao       = app->settings.get<int>("rpg_ssao");
    INFO.fsaa       = app->settings.get<int>("rpg_fsaa");

    world->configure();
    renderer->configure();
}


bool MainScene::handle(sf::Event _event) {
    if (_event.type == sf::Event::KeyPressed) {
        if (_event.key.code == sf::Keyboard::F) {
            bool val = app->settings.get<bool>("rpg_shadfilter");
            app->overlay.notify(string("shadow filtering set to ") + (val ? "LOW" : "HIGH"), 6u);
            app->settings.mod<bool>("rpg_shadfilter", !val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::S) {
            bool val = app->settings.get<bool>("rpg_shadlarge");
            app->overlay.notify(string("shadow resolution set to ") + (val ? "SMALL" : "LARGE"), 6u);
            app->settings.mod<bool>("rpg_shadlarge", !val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::C) {
            bool val = app->settings.get<bool>("rpg_vignette");
            app->overlay.notify(string("vignetting set to ") + (val ? "OFF" : "ON"), 6u);
            app->settings.mod<bool>("rpg_vignette", !val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::B) {
            bool val = app->settings.get<bool>("rpg_bloom");
            app->overlay.notify(string("hdr bloom set to ") + (val ? "OFF" : "ON"), 6u);
            app->settings.mod<bool>("rpg_bloom", !val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::L) {
            int val = app->settings.get<int>("rpg_shafts");
            app->overlay.notify(string("light shafts set to ") + (val ? (val>1 ? "OFF" : "HIGH") : "LOW"), 6u);
            app->settings.mod<int>("rpg_shafts", ++val == 3 ? 0 : val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::O) {
            int val = app->settings.get<int>("rpg_ssao");
            app->overlay.notify(string("ssao set to ") + (val ? (val>1 ? "OFF" : "HIGH") : "LOW"), 6u);
            app->settings.mod<int>("rpg_ssao", ++val == 3 ? 0 : val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::A) {
            int val = app->settings.get<int>("rpg_fsaa");
            app->overlay.notify(string("anti-aliasing set to ") + (val ? (val>1 ? "OFF" : "SMAA") : "FXAA"), 6u);
            app->settings.mod<int>("rpg_fsaa", ++val == 3 ? 0 : val);
            app->configure(); return true;
        }
        if (_event.key.code == sf::Keyboard::V) {
            int val = app->settings.get<int>("app_fpslimit");
            app->overlay.notify(string("framerate limit set to ") + (val ? (val>1 ? "NONE" : "VSYNC") : "75FPS"), 6u);
            app->settings.mod<int>("app_fpslimit", ++val == 3 ? 0 : val);
            app->configure(); return true;
        }
    }

//    if (_event.type == sf::Event::MouseButtonPressed) {
//        if (_event.mouseButton.button == sf::Mouse::Left) {
//            if (app->settings.get<bool>("console_active")) return false;
//            app->mouse_relatify(); scene->focused = true; return true;
//        }
//    }
    return false;
}
