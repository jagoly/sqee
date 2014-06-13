#include <libsqee/extra/handlers.hpp>

#include "app.hpp"
#include "mainmenu.hpp"
#include "scenehud.hpp"

using namespace sqt;

TestApp::TestApp() {
    texHolder.add_texture("player_still", "res/player_still.png");

    attach_handler("close", sq::HandlerPtr(new sqe::HandlerClose(this)));
    attach_handler("debug", sq::HandlerPtr(new sqe::HandlerDebug(this)));
    attach_handler("resize", sq::HandlerPtr(new sqe::HandlerResize(this)));
    attach_handler("framelimit", sq::HandlerPtr(new sqe::HandlerFramelimit(this)));

    attach_handler("mainmenu", sq::HandlerPtr(new sqe::HandlerMenu(this, "mainmenu")));
    append_scene("mainmenu", sq::ScenePtr(new SceneMainMenu(this)));
    append_scene("hud", sq::ScenePtr(new SceneHud(this)));
}
