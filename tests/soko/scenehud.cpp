#include <sstream>

#include "scenehud.hpp"
#include "scenemain.hpp"

using namespace sqt;

SceneHud::SceneHud(sq::Application* _app) : sq::Scene(_app){
    fontVector.push_back(sf::Font());
    fontVector.back().loadFromFile("res/DroidSans.ttf");
}

void SceneHud::update() {
    //pX = static_cast<SceneMain*>(&app->get_scene("main"))->pX;
    //pY = static_cast<SceneMain*>(&app->get_scene("main"))->pY;
}

void SceneHud::render(sf::RenderTarget& target, float ft) {
    static float fps = 60.f;
    fps = fps * 0.9f + 1.f / ft * 0.1f;

    static sf::Text textDisplay("", fontVector[0], 24);

    textDisplay.setString(" FPS: " + std::to_string(fps));

    target.draw(textDisplay);
}
