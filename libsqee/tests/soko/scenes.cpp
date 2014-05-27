#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;
using boost::get;

void SceneBack::render(float ft) {
    renderTex.clear(sf::Color(70, 20, 90));
    renderTex.display();
}

void SceneFore::render(float ft) {
    renderTex.clear(sf::Color::Transparent);

    static sf::Sprite playerSprite(textureHolder->get_texture("playerStill"), {0,0,64,64});

    float portion = get<float>(stage->stateMap["pSpeed"]) / stage->tickRate * 64.f;
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
            playerSprite.setTextureRect({0,64,64,64});
            posX += (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 2) {
            playerSprite.setTextureRect({64,0,64,64});
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
