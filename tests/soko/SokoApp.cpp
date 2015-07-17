#include <sqee/scenes/Basics.hpp>
#include <sqee/handlers/Basics.hpp>
#include <sqee/scripts/ConsoleScene.hpp>
#include <sqee/scripts/ConsoleHandler.hpp>
#include <sqee/app/SettingsMaps.hpp>

#include "Game.hpp"
#include "Title.hpp"
#include "Paused.hpp"
#include "SokoApp.hpp"

using namespace sqt;

SokoApp::SokoApp() {
    append_scene<GameScene>("game");
    append_scene<TitleScene>("title");
    append_scene<PausedScene>("paused");
    append_scene<sq::ConsoleScene>("console");
    append_scene<sq::BasicFPSScene>("fps");

    append_handler<sq::ConsoleHandler>("console");
    append_handler<sq::BasicCloseHandler>("close");
    append_handler<sq::BasicResizeHandler>("resize");
    append_handler<GameHandler>("game");
    append_handler<TitleHandler>("title");
    append_handler<PausedHandler>("paused");

    settings->mod<int>("framelimit", 2);
    settings->mod<bool>("resizable", true);
}
