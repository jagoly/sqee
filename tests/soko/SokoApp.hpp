#pragma once

#include <sqee/app/Application.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/DebugOverlay.hpp>

#include "Game.hpp"

namespace sqt {

class SokoApp : public sq::Application {
public:

    ~SokoApp();

private: //===================================================//

    void initialise(Vector<String> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    UniquePtr<sq::Window> mWindow;

    UniquePtr<sq::InputDevices> mInputDevices;

    UniquePtr<sq::DebugOverlay> mDebugOverlay;

    UniquePtr<GameScene> mGameScene;
};

}
