#include "MainScene.hpp"

#include "../render/Renderer.hpp"
#include "../systems/WorldStuff.hpp"

#include "../systems/Animation.hpp"
#include "../systems/Transform.hpp"
#include "../systems/Render.hpp"
#include "../systems/Culling.hpp"

#include "../api/ScriptAPI.hpp"

#include "Options.hpp"

#include <sqee/app/Event.hpp>
#include <sqee/debug/Misc.hpp>
#include <sqee/objects/Sound.hpp>

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

MainScene::MainScene ( const Options& options, wren::WrenPlusVM& vm,
                       sq::InputDevices& input, ResourceCaches& caches )
    : Scene(1.0 / 24.0)
    , options(options), mWrenVM(vm)
    , mInput(input), mCaches(caches)
{
    world = std::make_unique<WorldStuff>();
    renderer = std::make_unique<Renderer>(options);

    api = std::make_unique<ScriptAPI>(mCaches, *world);

    mPosCrnt = mPosNext = world->camera.get_position();

    //mEngine.add_global(chai::var(api.get()), "api");
}

MainScene::~MainScene() = default;

//============================================================================//

void MainScene::refresh_options_create()
{
    const Vec2F windowSize ( options.Window_Size );
    world->camera.set_aspect(windowSize.x / windowSize.y);
    world->camera.set_range(options.View_Distance);

    renderer->refresh_options();
}

//============================================================================//

void MainScene::update()
{
    mPosCrnt = mPosNext;

    //--------------------------------------------------------//

    if (mEnableInput == true)
    {
        using Key = sq::Keyboard_Key;

        Vec3F movement = { 0.f, 0.f, 0.f };

        if (mInput.is_pressed( Key::PageUp   )) movement.z += 0.06f;
        if (mInput.is_pressed( Key::PageDown )) movement.z -= 0.06f;

        if (mInput.is_pressed( Key::Arrow_Right )) movement += maths::rotate_z(Vec3F(+0.08f, 0.f, 0.f), mRotateZ);
        if (mInput.is_pressed( Key::Arrow_Left  )) movement += maths::rotate_z(Vec3F(-0.08f, 0.f, 0.f), mRotateZ);
        if (mInput.is_pressed( Key::Arrow_Up    )) movement += maths::rotate_z(Vec3F(0.f, +0.08f, 0.f), mRotateZ);
        if (mInput.is_pressed( Key::Arrow_Down  )) movement += maths::rotate_z(Vec3F(0.f, -0.08f, 0.f), mRotateZ);

        if (mInput.is_pressed(Key::Shift_L) || mInput.is_pressed(Key::Shift_R)) movement *= 4.f;

        mPosNext += movement;
    }

    //--------------------------------------------------------//

    sys::system_tick_animations(*world);
}

//============================================================================//

void MainScene::render(double)
{
    const float tickBlend = float(mAccumulation) * 24.f;

    //--------------------------------------------------------//

    if (mEnableInput == true)
    {
        const Vec2F mouseMove = Vec2F(mInput.cursor_to_centre());
        mRotateZ += mouseMove.x / 1600.f; mRotateX += mouseMove.y / 2400.f;
        mRotateX = maths::clamp(mRotateX, -0.23f, +0.23f);

        const QuatF rotation = QuatF(mRotateX, 0.f, mRotateZ);
        world->camera.set_position(maths::mix(mPosCrnt, mPosNext, tickBlend));
        world->camera.set_direction(rotation * Vec3F(0.f, 1.f, 0.f));

        //sq::Listener::set_position(world->camera.get_position());
        //sq::Listener::set_rotation(rotation);
    }

    //--------------------------------------------------------//

    world->deletedEntities.clear();

    sys::system_blend_animations(*world, tickBlend);

    //--------------------------------------------------------//

    sys::system_refresh_nesting(*world);
    sys::system_refresh_transforms(*world);
    sys::system_refresh_combo_sets(*world);
    sys::system_refresh_culling(*world);

    //--------------------------------------------------------//

    renderer->render_scene(*world);
}

//============================================================================//

void MainScene::set_input_enabled(bool enable)
{
    mEnableInput = enable;
    mInput.cursor_to_centre();
}
