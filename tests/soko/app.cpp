#include <libsqee/extra.hpp>

#include "app.hpp"
#include "scenemainmenu.hpp"
#include "scenemain.hpp"
#include "scenehud.hpp"

using namespace sqt;

TestApp::TestApp() {
    texHolder.add_texture("player_still", "res/player_still.png");

    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerClose(this)));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerDebug(this)));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerResize(this)));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerFramelimit(this)));

    append_scene("menu", std::shared_ptr<sq::Scene>(new SceneMainMenu(this)));
    append_scene("hud", std::shared_ptr<sq::Scene>(new SceneHud(this)));

    load_level("res/level1.json");
}

void TestApp::load_level(std::string filePath) {
    attach_handler(std::unique_ptr<sq::Handler>(new HandlerGame(this)));

    prepend_scene("main", std::shared_ptr<sq::Scene>(new SceneMain(this)));

    Level level(filePath);
    static_cast<SceneMain*>(&get_scene("main"))->load_level(level);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
