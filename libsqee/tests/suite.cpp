#include <iostream>

#include <libsqee/tests/suite.hpp>

using namespace sqt;
using boost::get;

TestApp::TestApp() {
    textureHolder.add_texture("playerStill", "test_soko/playerStill.png");
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

StageMain::StageMain() {
    tickRate = 32;
    dt = 0.03125f;
    stateMap.emplace(std::pair<std::string, int>("pDir", -1));
    stateMap.emplace(std::pair<std::string, int>("pX", 4));
    stateMap.emplace(std::pair<std::string, int>("pY", 4));
    stateMap.emplace(std::pair<std::string, float>("pMSpeed", 2.f));
    stateMap.emplace(std::pair<std::string, int>("pMoved", 0));
}

void StageMain::update() {
    if (get<int>(stateMap["pDir"]) == -1) {
        stateMap["pDir"] = sq::get_key_dir();
    } else {
        int pDir = get<int>(stateMap["pDir"]);
        stateMap["pMoved"] = get<int>(stateMap["pMoved"]) + 1;

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
            stateMap["pDir"] = sq::get_key_dir();
            return;
        }
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

    float portion = get<float>(stage->stateMap["pMSpeed"]) / stage->tickRate * 64.f;
    int pDir = get<int>(stage->stateMap["pDir"]);

    double posX = get<int>(stage->stateMap["pX"]) * 64.f;
    double posY = get<int>(stage->stateMap["pY"]) * 64.f;

    if (pDir != -1) {
        int frac = get<int>(stage->stateMap["pMoved"]);
        if (pDir == 0) {
            playerSprite.setTextureRect({0,0,64,64});
            posY -= (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 1) {
            playerSprite.setTextureRect({64,0,64,64});
            posX += (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 2) {
            playerSprite.setTextureRect({0,64,64,64});
            posY += (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 3) {
            playerSprite.setTextureRect({64,64,64,64});
            posX -= (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*portion) * (stage->accum / stage->dt);
        }
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
