#include <SFML/Window/Event.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>

#include "Paused.hpp"

using namespace sqt;

PausedScene::PausedScene(sq::Application* _app) : sq::Scene(_app) {

}


void PausedScene::update() {

}


void PausedScene::render() {

}


void PausedScene::refresh() {

}


bool PausedScene::handle(sf::Event _event) {
    return false;
}
