#include <iostream>

#include <libsqee/tests/suite.hpp>

using namespace sqt;

TestApp::TestApp() {
    running = true;
    textureHolder.add_texture("playerStill", "test_soko/playerStill.png");
}

bool HandlerGame::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            application->stageMap["main"]->stateMap["dir"] = 0;
            return true;
        } else
        if (event.key.code == sf::Keyboard::Right) {
            application->stageMap["main"]->stateMap["dir"] = 1;
            return true;
        } else
        if (event.key.code == sf::Keyboard::Down) {
            application->stageMap["main"]->stateMap["dir"] = 2;
            return true;
        } else
        if (event.key.code == sf::Keyboard::Left) {
            application->stageMap["main"]->stateMap["dir"] = 3;
            return true;
        }
    }
    return false;
}

StageMain::StageMain() {
    stateMap.emplace(std::pair<std::string, int>("dir", 0));
}

StageHud::StageHud() {
    stateMap.emplace(std::pair<std::string, float>("runTime", 0.0f));
}

void StageHud::update() {
    stateMap["runTime"] = application->clock.getElapsedTime().asSeconds();
}

void SceneBack::render() {
    renderTex.clear(sf::Color::Color(70, 20, 90));
    renderTex.display();
}

void SceneFore::render() {
    renderTex.clear(sf::Color::Transparent);

    static sf::Sprite playerSprite(textureHolder->get_texture("playerStill"));

    if (stage->stateMap["dir"] == StageHud::State(0)) {
        playerSprite.setTextureRect({0, 0, 64, 64});
    } else
    if (stage->stateMap["dir"] == StageHud::State(1)) {
        playerSprite.setTextureRect({0, 64, 64, 64});
    } else
    if (stage->stateMap["dir"] == StageHud::State(2)) {
        playerSprite.setTextureRect({64, 0, 64, 64});
    } else
    if (stage->stateMap["dir"] == StageHud::State(3)) {
        playerSprite.setTextureRect({64, 64, 64, 64});
    }

    renderTex.draw(playerSprite);
    renderTex.display();
}

SceneHud::SceneHud() {
    fontVector.push_back(sf::Font());
    fontVector[0].loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render() {
    static sf::Text timeDisplay("", fontVector[0], 24);

    renderTex.clear(sf::Color::Transparent);

    std::string time = std::to_string(boost::get<float>(stage->stateMap["runTime"]));
    timeDisplay.setString("Running for "+time);
    renderTex.draw(timeDisplay);

    renderTex.display();
}
