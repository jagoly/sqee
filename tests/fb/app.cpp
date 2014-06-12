#include <libsqee/extra/handlers.hpp>
#include "app.hpp"
#include "menus.hpp"
#include "scenehud.hpp"

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

    attach_handler("close", std::shared_ptr<sq::Handler>(new sqe::HandlerClose(this)));
    attach_handler("debug", std::shared_ptr<sq::Handler>(new sqe::HandlerDebug(this)));
    attach_handler("resize", std::shared_ptr<sq::Handler>(new sqe::HandlerResize(this)));
    attach_handler("framelimit", std::shared_ptr<sq::Handler>(new sqe::HandlerFramelimit(this)));

    append_scene("hud", std::shared_ptr<sq::Scene>(new SceneHud(this)));
}
