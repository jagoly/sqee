#include <libsqee/extra/handlers.hpp>
#include "app.hpp"
#include "menus.hpp"
#include "scenehud.hpp"
#include "scenegame.hpp"

using namespace sqt;

TestApp::TestApp() {
    texHolder.add_texture("digitssmall", "res/digitssmall.png");
    texHolder.add_texture("digitsmedium", "res/digitsmedium.png");
    texHolder.add_texture("digitslarge", "res/digitslarge.png");
    texHolder.add_texture("medals", "res/medals.png");
    texHolder.add_texture("floor", "res/floor.png");
    texHolder.add_texture("title", "res/title.png");
    texHolder.add_texture("start", "res/start.png");
    texHolder.add_texture("getready", "res/getready.png");
    texHolder.add_texture("gameover", "res/gameover.png");
    texHolder.add_texture("new", "res/new.png");
    texHolder.add_texture("scoreboard", "res/scoreboard.png");
    texHolder.add_texture("playbutton", "res/playbutton.png");
    texHolder.add_texture("scorebutton", "res/scorebutton.png");

    attach_handler("close", sq::HandlerPtr(new sqe::HandlerClose(this)));
    attach_handler("debug", sq::HandlerPtr(new sqe::HandlerDebug(this)));
    attach_handler("resize", sq::HandlerPtr(new sqe::HandlerResize(this)));
    attach_handler("framelimit", sq::HandlerPtr(new sqe::HandlerFramelimit(this)));

    append_scene("game", sq::ScenePtr(new SceneGame(this)));
    attach_handler("game", sq::HandlerPtr(new HandlerGame(this)));
    append_scene("hud", sq::ScenePtr(new SceneHud(this)));
}
