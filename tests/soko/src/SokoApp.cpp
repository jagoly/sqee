#include <sqee/gl/Context.hpp>

#include "SokoApp.hpp"

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

SokoApp::~SokoApp() = default;

//============================================================================//

void SokoApp::initialise(std::vector<std::string> /*args*/)
{
    mWindow = std::make_unique<sq::Window>("SQEE Sokoban", Vec2U(800u, 600u));

    mInputDevices = std::make_unique<sq::InputDevices>(*mWindow);

    mDebugOverlay = std::make_unique<sq::DebugOverlay>();

    mAudioContext = std::make_unique<sq::AudioContext>();

    mGameScene = std::make_unique<GameScene>(*mInputDevices);

    mSound = std::make_unique<sq::Sound>(*mAudioContext);
    mSound->load_from_file("assets/sounds/Jump.wav");

    refresh_options();
}

//============================================================================//

void SokoApp::update(double elapsed)
{
    auto& context = sq::Context::get();

    for (auto event : mWindow->fetch_events())
        handle_event(event);

    context.set_ViewPort(mWindow->get_window_size());
    context.bind_framebuffer_default(sq::FboTarget::Both);

    mGameScene->update_and_render(elapsed);

    mDebugOverlay->update_and_render(elapsed);

    mWindow->swap_buffers();
}

//============================================================================//

void SokoApp::refresh_options()
{
    mAudioContext->play_sound(*mSound, sq::SoundGroup::Menu, 1.f, false);

    sq::Context::get().set_ViewPort(mWindow->get_window_size());
    mGameScene->refresh_options_create();
}

//============================================================================//

void SokoApp::handle_event(sq::Event event)
{
    using Type = sq::Event::Type;
    using Key = sq::Keyboard_Key;

    const auto& [type, data] = event;

    if (type == Type::Window_Close)
    {
        mReturnCode = 0;
        return;
    }

    if (type == Type::Window_Resize)
    {
        refresh_options();
        return;
    }

    if (type == Type::Keyboard_Press && data.keyboard.key == Key::Menu)
    {
        mDebugOverlay->toggle_active();
        return;
    }

    if (type == Type::Keyboard_Press && data.keyboard.key == Key::V)
    {
        const bool value = !mWindow->get_vsync_enabled();
        mWindow->set_vsync_enabled(value);

        if (value) mDebugOverlay->notify("vsync set to ON");
        else mDebugOverlay->notify("vsync set to OFF");

        return;
    }

    mGameScene->handle_event(event);
}
