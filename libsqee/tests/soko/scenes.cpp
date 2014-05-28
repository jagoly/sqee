#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

SceneBack::SceneBack(sq::Stage& sta) {
    stage = static_cast<StageMain*>(&sta);
}

void SceneBack::render(float ft) {
    renderTex.clear(sf::Color(70, 20, 90));
    renderTex.display();
}

SceneFore::SceneFore(sq::Stage& sta) {
    stage = static_cast<StageMain*>(&sta);
}

void SceneFore::render(float ft) {
    renderTex.clear(sf::Color::Transparent);

    static sf::Sprite playerSprite(textureHolder->get_texture("playerStill"), {0,0,64,64});

    float portion = stage->pSpeed / stage->tickRate * 64.f;
    int pDir = stage->pDir;

    double posX = stage->pX * 64.f;
    double posY = stage->pY * 64.f;

    if (pDir != -1) {
        int frac = stage->pMoved;
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

SceneHud::SceneHud(sq::Stage& sta) {
    stage = static_cast<StageHud*>(&sta);
    fontVector.push_back(sf::Font());
    fontVector[0].loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render(float ft) {
    static sf::Text timeDisplay("", fontVector[0], 24);

    renderTex.clear(sf::Color::Transparent);

    std::string time = std::to_string(1.f / ft);
    timeDisplay.setString("FPS: "+time);
    renderTex.draw(timeDisplay);

    renderTex.display();
}
