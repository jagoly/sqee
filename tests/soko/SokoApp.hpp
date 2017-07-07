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

    void initialise(std::vector<string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    unique_ptr<sq::Window> mWindow;

    unique_ptr<sq::InputDevices> mInputDevices;

    unique_ptr<sq::DebugOverlay> mDebugOverlay;

    unique_ptr<GameScene> mGameScene;
};

}
