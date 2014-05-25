#include <iostream>

#include <libsqee/tests/suite.hpp>

using namespace sqt;
using boost::get;

TestApp::TestApp() {
    tickRate = 10;
    textureHolder.add_texture("playerStill", "test_soko/playerStill.png");
}

bool HandlerGame::handle(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        bool pMoveAllow = get<int>(application->stageMap["main"]->stateMap["pDir"]) == -1;
        if (event.key.code == sf::Keyboard::Up) {
            if (pMoveAllow) {
                application->stageMap["main"]->stateMap["pDir"] = 0;
            }
            return true;
        } else
        if (event.key.code == sf::Keyboard::Right) {
            if (pMoveAllow) {
                application->stageMap["main"]->stateMap["pDir"] = 1;
            }
            return true;
        } else
        if (event.key.code == sf::Keyboard::Down) {
            if (pMoveAllow) {
                application->stageMap["main"]->stateMap["pDir"] = 2;
            }
            return true;
        } else
        if (event.key.code == sf::Keyboard::Left) {
            if (pMoveAllow) {
                application->stageMap["main"]->stateMap["pDir"] = 3;
            }
            return true;
        }
    }
    return false;
}

StageMain::StageMain() {
    stateMap.emplace(std::pair<std::string, int>("pDir", -1));
    stateMap.emplace(std::pair<std::string, float>("pMRemains", 1.f));
    stateMap.emplace(std::pair<std::string, int>("pX", 4));
    stateMap.emplace(std::pair<std::string, int>("pY", 4));
    stateMap.emplace(std::pair<std::string, float>("pMSpeed", 2.f));
}

void StageMain::update() {
    static float pMDif = get<float>(stateMap["pMSpeed"]) / application->tickRate;
    if (get<int>(stateMap["pDir"]) != -1) {
        stateMap["pMRemains"] = get<float>(stateMap["pMRemains"]) - pMDif;
        if (get<float>(stateMap["pMRemains"]) < pMDif) {
            if (get<int>(stateMap["pDir"]) == 0) {
                stateMap["pY"] = get<int>(stateMap["pY"]) - 1;
            } else
            if (get<int>(stateMap["pDir"]) == 1) {
                stateMap["pX"] = get<int>(stateMap["pX"]) + 1;
            } else
            if (get<int>(stateMap["pDir"]) == 2) {
                stateMap["pY"] = get<int>(stateMap["pY"]) + 1;
            } else
            if (get<int>(stateMap["pDir"]) == 3) {
                stateMap["pX"] = get<int>(stateMap["pX"]) - 1;
            }
            stateMap["pMRemains"] = 1.f;
            stateMap["pDir"] = -1;
        }
    }
}

StageHud::StageHud() {
}

void StageHud::update() {
}

void SceneBack::render(float dt) {
    renderTex.clear(sf::Color::Color(70, 20, 90));
    renderTex.display();
}

void SceneFore::render(float dt) {
    renderTex.clear(sf::Color::Transparent);

    static float cTime;
    static float speed = get<float>(stage->stateMap["pMSpeed"]);
    static sf::Sprite playerSprite(textureHolder->get_texture("playerStill"), {0,0,64,64});

    float posX = get<int>(stage->stateMap["pX"]) * 64;
    float posY = get<int>(stage->stateMap["pY"]) * 64;

    if (get<int>(stage->stateMap["pDir"]) != -1) {
        cTime += dt;

        if (get<int>(stage->stateMap["pDir"]) == 0) {
            playerSprite.setTextureRect({0,0,64,64});
            posY -= 64 * speed / cTime;
        } else
        if (get<int>(stage->stateMap["pDir"]) == 1) {
            playerSprite.setTextureRect({0,64,64,64});
            posX += 64 * speed / cTime;
        } else
        if (get<int>(stage->stateMap["pDir"]) == 2) {
            playerSprite.setTextureRect({64,0,64,64});
            posY += 64 * speed / cTime;
        } else
        if (get<int>(stage->stateMap["pDir"]) == 3) {
            playerSprite.setTextureRect({64,64,64,64});
            posX -= 64 * speed / cTime;
        }
        std::cout << cTime << std::endl;
    } else {
        cTime = 0.f;
        std::cout << "done" << std::endl;
    }

    playerSprite.setPosition(posX, posY);

    renderTex.draw(playerSprite);
    renderTex.display();
}

SceneHud::SceneHud() {
    fontVector.push_back(sf::Font());
    fontVector[0].loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render(float dt) {
    static sf::Text timeDisplay("", fontVector[0], 24);
    static float pt = 0.f;

    renderTex.clear(sf::Color::Transparent);

    std::string time = std::to_string(1/(dt-pt));
    timeDisplay.setString("FPS: "+time);
    renderTex.draw(timeDisplay);

    renderTex.display();
}
