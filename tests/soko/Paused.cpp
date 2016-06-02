#include <SFML/Window/Event.hpp>

#include "SokoApp.hpp"
#include "Paused.hpp"

using namespace sqt;

PausedScene::PausedScene(SokoApp& _app) : sq::Scene(1.0 / 30.0), app(_app) {

}

void PausedScene::update_options() {

}

void PausedScene::tick() {

}

void PausedScene::render() {

}

bool PausedScene::handle(sf::Event _event) {
    return false;
}
