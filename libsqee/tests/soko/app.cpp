#include <libsqee/extra.hpp>

#include <libsqee/tests/soko/app.hpp>
#include <libsqee/tests/soko/stages.hpp>
#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

TestApp::TestApp() {
    textureHolder.add_texture("player_still", "test_soko/player_still.png");

    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerClose));
    attach_handler(std::unique_ptr<sq::Handler>(new sqe::HandlerDebug));

    add_stage("hud", std::unique_ptr<sq::Stage>(new StageHud));
    append_scene(std::unique_ptr<sq::Scene>(new SceneHud(get_stage("hud"), textureHolder)));

    load_level("test_soko/level1.json");
}

void TestApp::load_level(std::string filePath) {
    level = new Level(filePath);
    attach_handler(std::unique_ptr<sq::Handler>(new HandlerGame));

    StageMain* stageMain = new StageMain;
    stageMain->load_level(*level);

    add_stage("main", std::unique_ptr<sq::Stage>(stageMain));

    prepend_scene(std::unique_ptr<sq::Scene>(new SceneFore(get_stage("main"), textureHolder)));
    prepend_scene(std::unique_ptr<sq::Scene>(new SceneBack(get_stage("main"), textureHolder)));

   // get_stage("main").load_level(&level);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}
