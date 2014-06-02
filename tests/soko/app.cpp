#include <libsqee/extra.hpp>

#include "app.hpp"
#include "stages.hpp"
#include "scenes.hpp"

using namespace sqt;

TestApp::TestApp() {
    textureHolder.add_texture("player_still", "res/player_still.png");

    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerClose));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerDebug));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerResize));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerFramelimit));

    add_stage("hud", std::unique_ptr<sq::Stage>(new StageHud));
    append_scene(std::unique_ptr<sq::Scene>(new SceneHud(get_stage("hud"), textureHolder)));

    load_level("res/level1.json");
}

void TestApp::load_level(std::string filePath) {
    attach_handler(std::unique_ptr<sq::Handler>(new HandlerGame));

    add_stage("main", std::unique_ptr<sq::Stage>(new StageMain));
    static_cast<StageMain*>(&get_stage("main"))->load_level(new Level(filePath));

    prepend_scene(std::unique_ptr<sq::Scene>(new SceneMain(get_stage("main"), textureHolder)));
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
