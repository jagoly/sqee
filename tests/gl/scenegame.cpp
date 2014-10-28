#include <sqee/gl/gl.hpp>

#include "scenegame.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    camera.init({0, 0, 2}, {0, 0, 0}, 4, 3, 1.17f, 0.1f, 100.f);
    camera.update_projMat();
    camera.update_viewMat();
}

void SceneGame::update() {

}

void SceneGame::render(float) {

}

bool HandlerGame::handle(const sf::Event&) {
    return false;
}
