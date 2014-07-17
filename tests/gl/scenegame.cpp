#include "scenegame.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;
}

void SceneGame::update() {

}

void SceneGame::render(sf::RenderTarget& target, float) {

}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

}
