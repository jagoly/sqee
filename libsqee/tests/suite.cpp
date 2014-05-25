#include <iostream>

#include <libsqee/tests/suite.hpp>

using namespace sqt;
using boost::get;

TestApp::TestApp() {
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
    tickRate = 2;
    dt = 0.5f;
    stateMap.emplace(std::pair<std::string, int>("pDir", -1));
    stateMap.emplace(std::pair<std::string, int>("pX", 4));
    stateMap.emplace(std::pair<std::string, int>("pY", 4));
    stateMap.emplace(std::pair<std::string, float>("pMSpeed", 1.f));
    stateMap.emplace(std::pair<std::string, int>("pMoved", 0));
}

void StageMain::update() {
    std::cout << "called" << std::endl;
    int pDir = get<int>(stateMap["pDir"]);
    if (pDir != -1) {
        std::cout << stateMap["pMoved"] << std::endl;
        if (get<int>(stateMap["pMoved"]) == int(tickRate / get<float>(stateMap["pMSpeed"]))) {
            if (pDir == 0) {
                stateMap["pY"] = get<int>(stateMap["pY"]) - 1;
            } else
            if (pDir == 1) {
                stateMap["pX"] = get<int>(stateMap["pX"]) + 1;
            } else
            if (pDir == 2) {
                stateMap["pY"] = get<int>(stateMap["pY"]) + 1;
            } else
            if (pDir == 3) {
                stateMap["pX"] = get<int>(stateMap["pX"]) - 1;
            }
            stateMap["pMoved"] = 0;
            stateMap["pDir"] = -1;
        }
        stateMap["pMoved"] = get<int>(stateMap["pMoved"]) + 1;
    }
}

StageHud::StageHud() {
    tickRate = 2;
}

void StageHud::update() {
}

void SceneBack::render(float ft) {
    renderTex.clear(sf::Color::Color(70, 20, 90));
    renderTex.display();
}

void SceneFore::render(float ft) {
    renderTex.clear(sf::Color::Transparent);

    static sf::Sprite playerSprite(textureHolder->get_texture("playerStill"), {0,0,64,64});

    static int part = stage->dt * 64;

    double posX = get<int>(stage->stateMap["pX"]) * 64.f;
    double posY = get<int>(stage->stateMap["pY"]) * 64.f;

    int pDir = get<int>(stage->stateMap["pDir"]);

    if (pDir != -1) {
        int frac = get<int>(stage->stateMap["pMoved"]);
        if (pDir == 0) {
            playerSprite.setTextureRect({0,0,64,64});
            posY -= (frac*32.f) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*32.f) * (stage->accum / stage->dt);
        } else
        if (pDir == 1) {
            playerSprite.setTextureRect({0,64,64,64});
            posX += 64.f * stage->accum / 0.1;
        } else
        if (pDir == 2) {
            playerSprite.setTextureRect({64,0,64,64});
            posY += 64.f * stage->accum / 0.1;
        } else
        if (pDir == 3) {
            playerSprite.setTextureRect({64,64,64,64});
            posX -= 64.f * stage->accum / 0.1;
        }
        std::cout << "extra: " << std::endl;
        std::cout << posX << " " << posY << std::endl;
    }

    playerSprite.setPosition(posX, posY);

    renderTex.draw(playerSprite);
    renderTex.display();
}

SceneHud::SceneHud() {
    fontVector.push_back(sf::Font());
    fontVector[0].loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render(float ft) {
    static sf::Text timeDisplay("", fontVector[0], 24);
    static float pt = 0.f;

    renderTex.clear(sf::Color::Transparent);

    std::string time = std::to_string(1/(ft-pt));
    timeDisplay.setString("FPS: "+time);
    renderTex.draw(timeDisplay);

    renderTex.display();
}

/*
 *
tr = 8
dt = 0.125

X    = 5
pM   = 3
acum = 0.05


5*64 + 3*8

5*64 + (3*8) * ((0.125 - 0.05) / 0.125) + (4*8) * (0.05 / 0.125)

5*64 + 4*8

*/
