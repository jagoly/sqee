#include <sqee/app/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/SettingsMaps.hpp>

#include "Paused.hpp"

using namespace sqt;

PausedScene::PausedScene(sq::Application* _app) : sq::Scene(_app) {

}


void PausedScene::update() {

}


void PausedScene::render(float _ft) {

}


void PausedScene::update_settings() {

}


bool PausedHandler::handle(const sf::Event& _event) {
    return false;
}
