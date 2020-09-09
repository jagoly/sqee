#pragma once

#include <sqee/app/Application.hpp>
#include <sqee/app/Window.hpp>
#include <sqee/app/InputDevices.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/WrenPlus.hpp>

#include "main/MainScene.hpp"
#include "main/Options.hpp"
#include "main/ResourceCaches.hpp"
#include "main/LuaConsole.hpp"

//============================================================================//

namespace sqt {

class RpgApp final : public sq::Application
{
public: //====================================================//

    RpgApp();

    ~RpgApp() override;

private: //===================================================//

    void initialise(std::vector<std::string> args) override;

    void update(double elapsed) override;

    //--------------------------------------------------------//

    void handle_event(sq::Event event);

    void refresh_options();

    //--------------------------------------------------------//

    Options mOptions;

    //--------------------------------------------------------//

    std::unique_ptr<wren::WrenPlusVM> mWrenVM;

    std::unique_ptr<sq::Window> mWindow;

    std::unique_ptr<sq::InputDevices> mInputDevices;

    std::unique_ptr<WrenConsole> mWrenConsole;

    std::unique_ptr<sq::DebugOverlay> mDebugOverlay;

    std::unique_ptr<ResourceCaches> mResourceCaches;

    std::unique_ptr<MainScene> mMainScene;
};

} // namespace sqt
