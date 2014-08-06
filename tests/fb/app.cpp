#include <libsqee/extra/handlers.hpp>
#include "app.hpp"
#include "menus.hpp"
#include "scenegame.hpp"

namespace sqt {

TestApp::TestApp() : sq::Application(612, 714, 0, true, true, false, "SQEE Flappy Test") {
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

    soundManager.add_sound("hit", "res/sound/hit.ogg");
    soundManager.add_sound("wing", "res/sound/wing.ogg");
    soundManager.add_sound("point", "res/sound/point.ogg");

    attach_handler("close", sq::HandlerPtr(new sq::HandlerClose(this)));
    attach_handler("debug", sq::HandlerPtr(new sq::HandlerDebug(this)));
    attach_handler("resize", sq::HandlerPtr(new sq::HandlerResize(this)));
    attach_handler("framelimit", sq::HandlerPtr(new sq::HandlerFramelimit(this)));

    append_scene("game", sq::ScenePtr(new SceneGame(this)));
    append_scene("menu", sq::ScenePtr(new SceneMenus(this)));
    attach_handler("game", sq::HandlerPtr(new HandlerGame(this)));
}

}
