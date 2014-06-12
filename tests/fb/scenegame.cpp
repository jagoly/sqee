#include <iostream>

#include "scenegame.hpp"
#include "menus.hpp"
#include "helpers.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1.f/120.f;

    active = true;
}

void SceneGame::update() {
    if (active) {

    }
}

void SceneGame::render(sf::RenderTarget& target, float ft) {

}
