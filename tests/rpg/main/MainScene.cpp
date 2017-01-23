#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/scripts/ChaiEngine.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/sound/Sound.hpp>
#include <sqee/debug/Misc.hpp>

#include "../RpgApp.hpp"
#include "../world/World.hpp"
#include "../render/Renderer.hpp"
#include "Scripting.hpp"
#include "MainScene.hpp"

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

MainScene::MainScene(RpgApp& app) : sq::Scene(1.0 / 24.0), mRpgApp(app)
{
    auto& mbus = mRpgApp.get_message_bus();

    mbus.register_type<msg::Enable_SkyBox>();
    mbus.register_type<msg::Enable_Ambient>();
    mbus.register_type<msg::Enable_SkyLight>();

    mbus.register_type<msg::Disable_SkyBox>();
    mbus.register_type<msg::Disable_Ambient>();
    mbus.register_type<msg::Disable_SkyLight>();

    mbus.register_type<msg::Debug_1>();
    mbus.register_type<msg::Debug_2>();
    mbus.register_type<msg::Debug_3>();
    mbus.register_type<msg::Debug_4>();

    world = std::make_unique<World>(mbus);
    renderer = std::make_unique<Renderer>(mbus);

    mPosCrnt = world->camera->PROP_position;
    mPosNext = world->camera->PROP_position;

    chaiscript_setup_world(app.get_chai_engine());
    chaiscript_setup_api(app.get_chai_engine());
    chaiscript_setup_systems(app.get_chai_engine());
    chaiscript_setup_messages(app.get_chai_engine());

    auto& chaiEngine = mRpgApp.get_chai_engine();

    chaiEngine.add_global(chai::var(&mbus), "mbus");
    chaiEngine.add_global(chai::var(world.get()), "world");

    on_Toggle_Chai_Console.func = [this](auto)
    { this->mRpgApp.mouse_centre(); };

    mbus.subscribe_back(on_Toggle_Chai_Console);
}

//============================================================================//

void MainScene::update_options()
{
    world->update_options();
    renderer->update_options();
}

//============================================================================//

bool MainScene::handle(sf::Event event)
{
    return false;
}

//============================================================================//

void MainScene::tick()
{
    using KB = sf::Keyboard;
    mPosCrnt = mPosNext;

    if (mRpgApp.get_chai_console().is_active() == false)
    {
        if (KB::isKeyPressed(KB::PageUp)) mPosNext.z += 0.06f;
        if (KB::isKeyPressed(KB::PageDown)) mPosNext.z -= 0.06f;

        if (KB::isKeyPressed(KB::Right) && !KB::isKeyPressed(KB::Left))
            mPosNext += maths::rotate_z(Vec3F(+0.08f, 0.f, 0.f), mRotateZ);
        if (KB::isKeyPressed(KB::Left) && !KB::isKeyPressed(KB::Right))
            mPosNext += maths::rotate_z(Vec3F(-0.08f, 0.f, 0.f), mRotateZ);

        if (KB::isKeyPressed(KB::Up) && !KB::isKeyPressed(KB::Down))
            mPosNext += maths::rotate_z(Vec3F(0.f, +0.08f, 0.f), mRotateZ);
        if (KB::isKeyPressed(KB::Down) && !KB::isKeyPressed(KB::Up))
            mPosNext += maths::rotate_z(Vec3F(0.f, -0.08f, 0.f), mRotateZ);
    }

    world->tick();
}

//============================================================================//

void MainScene::render()
{
    const float tickPercent = float(accumulation) * 24.f;

    world->tickPercent = tickPercent;
    renderer->tickPercent = tickPercent;

    if (mRpgApp.get_chai_console().is_active() == false)
    {
        const Vec2F mouseMove = mRpgApp.mouse_centre();
        mRotateZ += mouseMove.x / 1600.f; mRotateX += mouseMove.y / 2400.f;
        mRotateX = maths::clamp(mRotateX, -0.23f, +0.23f);

        const QuatF rotation = QuatF(mRotateX, 0.f, mRotateZ);
        world->camera->PROP_position = maths::mix(mPosCrnt, mPosNext, tickPercent);
        world->camera->PROP_direction = rotation * Vec3F(0.f, 1.f, 0.f);

        sq::Listener::set_position(world->camera->PROP_position);
        sq::Listener::set_rotation(rotation);
    }

    auto& stuff = sys::static_WorldStuff();

    sys::system_blend_animations(stuff, tickPercent);

    world->update();

    renderer->render_scene(world->sceneData);
}
