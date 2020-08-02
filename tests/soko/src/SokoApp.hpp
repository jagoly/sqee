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

    void initialise(std::vector<std::string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    std::unique_ptr<sq::Window> mWindow;

    std::unique_ptr<sq::InputDevices> mInputDevices;

    std::unique_ptr<sq::DebugOverlay> mDebugOverlay;

    std::unique_ptr<GameScene> mGameScene;
};

}
