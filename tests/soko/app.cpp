#include <libsqee/extra.hpp>

#include "app.hpp"
#include "scenemainmenu.hpp"
#include "handlermainmenu.hpp"
#include "scenemain.hpp"
#include "scenehud.hpp"

using namespace sqt;

TestApp::TestApp() {
    texHolder.add_texture("player_still", "res/player_still.png");

    attach_handler("close", std::shared_ptr<sq::Handler>(new sqe::HandlerClose(this)));
    attach_handler("debug", std::shared_ptr<sq::Handler>(new sqe::HandlerDebug(this)));
    attach_handler("resize", std::shared_ptr<sq::Handler>(new sqe::HandlerResize(this)));
    attach_handler("framelimit", std::shared_ptr<sq::Handler>(new sqe::HandlerFramelimit(this)));

    //prepend_scene("main", std::shared_ptr<sq::Scene>(new SceneMain(this)));
    append_scene("menu", std::shared_ptr<sq::Scene>(new SceneMainMenu(this)));
    append_scene("hud", std::shared_ptr<sq::Scene>(new SceneHud(this)));
    attach_handler("mainmenu", std::shared_ptr<sq::Handler>(new HandlerMainMenu(this)));
}
