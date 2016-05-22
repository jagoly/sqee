#include <SFML/Window/Event.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>

#include "Title.hpp"

using namespace sqt;

TitleScene::TitleScene(sq::Application* _app) : sq::Scene(_app) {

}


void TitleScene::configure() {

}


void TitleScene::tick() {

}


void TitleScene::render() {

}


bool TitleScene::handle(sf::Event _event) {
    return false;
}
