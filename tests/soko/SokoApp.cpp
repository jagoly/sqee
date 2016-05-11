#include <sqee/app/Settings.hpp>

#include "Game.hpp"
#include "Title.hpp"
#include "Paused.hpp"
#include "SokoApp.hpp"

using namespace sqt;

SokoApp::SokoApp() {
    append_scene<GameScene>("game");
    append_scene<TitleScene>("title");
    append_scene<PausedScene>("paused");

    settings.mod<int>("app_fpslimit", 2);
    settings.mod<bool>("app_resizable", true);
}
