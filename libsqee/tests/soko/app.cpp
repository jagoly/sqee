#include <libsqee/extra.hpp>

#include <libsqee/tests/soko/app.hpp>
#include <libsqee/tests/soko/stages.hpp>
#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

TestApp::TestApp() {
    textureHolder.add_texture("playerStill", "test_soko/playerStill.png");

    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerClose));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerDebug));
    attach_handler(std::unique_ptr<sq::Handler>(new HandlerGame));

    add_stage("main", std::unique_ptr<sq::Stage>(new StageMain));
    add_stage("hud", std::unique_ptr<sq::Stage>(new StageHud));

    append_scene(std::unique_ptr<sq::Scene>(new SceneBack(get_stage("main"))));
    append_scene(std::unique_ptr<sq::Scene>(new SceneFore(get_stage("main"))));
    append_scene(std::unique_ptr<sq::Scene>(new SceneHud(get_stage("hud"))));
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
