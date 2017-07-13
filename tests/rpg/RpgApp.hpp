#pragma once

#include <sqee/app/Application.hpp>
#include <sqee/app/ChaiEngine.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/ChaiConsole.hpp>
#include <sqee/app/DebugOverlay.hpp>

#include "main/MainScene.hpp"
#include "main/Options.hpp"
#include "main/ResourceCaches.hpp"

//============================================================================//

namespace sqt {

class RpgApp final : public sq::Application
{
public: //====================================================//

    RpgApp(); ///< Constructor.

    ~RpgApp(); ///< Destructor.

private: //===================================================//

    void initialise(std::vector<string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    Options mOptions;

    //--------------------------------------------------------//

    unique_ptr<sq::ChaiEngine> mChaiEngine;

    unique_ptr<sq::Window> mWindow;

    unique_ptr<sq::InputDevices> mInputDevices;

    unique_ptr<sq::ChaiConsole> mChaiConsole;

    unique_ptr<sq::DebugOverlay> mDebugOverlay;

    unique_ptr<ResourceCaches> mResourceCaches;

    unique_ptr<MainScene> mMainScene;
};

} // namespace sqt
