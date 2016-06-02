#include <SFML/Window/Event.hpp>

#include "SokoApp.hpp"
#include "Title.hpp"

using namespace sqt;

TitleScene::TitleScene(SokoApp& _app) : sq::Scene(1.0 / 30.0), app(_app) {

}

void TitleScene::update_options() {

}

void TitleScene::tick() {

}

void TitleScene::render() {

}

bool TitleScene::handle(sf::Event _event) {
    return false;
}
