#include <rp3d/engine/DynamicsWorld.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/scripts/ChaiScript.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>

#include <sqee/debug/Misc.hpp>

#include "../RpgApp.hpp"
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

MainScene::MainScene(RpgApp& _app) : sq::Scene(1.0 / 24.0), app(_app) {

    world.reset(new World(app.options));
    renderer.reset(new Renderer(app.options, *world));

    posCrnt = posNext = world->get_Camera().PROP_position;

    chaiscript_setup_world(*app.chaiEngine, *world);
    chaiscript_setup_components(*app.chaiEngine, *world);
    chaiscript_setup_functions(*app.chaiEngine, *world);

    app.chaiEngine->add_global(chai::var(world.get()), "world");

    app.console->onHideFuncs.append("MainScene_reset_mouse",
        std::bind<void>(&sq::Application::mouse_centre, &app));

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

void MainScene::update_options() {
//    TX.pshadA->allocate_storage(INFO.halfSize, false);
//    TX.pshadB->allocate_storage(INFO.halfSize, false);
//    TX.partMain->allocate_storage(INFO.halfSize, false);
//    TX.partDpSt->allocate_storage(INFO.halfSize, false);
//    TX.hdrPart->allocate_storage(INFO.halfSize, false);

    world->get_Camera().size = Vec2F(app.options.WindowSize);

    world->update_options();
    renderer->update_options();
}

void MainScene::tick() {
    using KB = sf::Keyboard;
    posCrnt = posNext;

    if (app.console->active == false) {
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
    float tickPercent = accumulation * 24.0;

    world->tickPercent = tickPercent;
    renderer->tickPercent = tickPercent;

    if (app.console->active == false) {
        Vec2F mMove = app.mouse_centre();
        rotZ = rotZ + mMove.x / 1600.f;
        rotX = maths::clamp(rotX + mMove.y / 2400.f, -0.23f, 0.23f);
        world->get_Camera().PROP_direction = maths::rotate_z(maths::rotate_x(Vec3F(0.f, 1.f, 0.f), rotX), rotZ);
        world->get_Camera().PROP_position = maths::mix(posCrnt, posNext, tickPercent);
    }

    world->update();

    renderer->render_scene();

    gl::BindProgramPipeline(0u);
    gl::BindVertexArray(0u);
}



bool MainScene::handle(sf::Event _event) {
    return false;
}
