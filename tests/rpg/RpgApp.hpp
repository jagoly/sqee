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

    RpgApp();

    ~RpgApp() override;

private: //===================================================//

    void initialise(Vector<String> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    Options mOptions;

    //--------------------------------------------------------//

    UniquePtr<sq::ChaiEngine> mChaiEngine;

    UniquePtr<sq::Window> mWindow;

    UniquePtr<sq::InputDevices> mInputDevices;

    UniquePtr<sq::ChaiConsole> mChaiConsole;

    UniquePtr<sq::DebugOverlay> mDebugOverlay;

    UniquePtr<ResourceCaches> mResourceCaches;

    UniquePtr<MainScene> mMainScene;
};

} // namespace sqt
