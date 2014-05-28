#include <iostream>

#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

SceneBack::SceneBack(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageMain*>(&sta);
    textureHolder = &texHolder;

    for (auto path : stage->texPathVec) {
        textureHolder->add_texture("bg_" + std::to_string(path.first), path.second);
    }
}

void SceneBack::render(float ft) {
    renderTex.clear(sf::Color(70, 20, 90));
    static std::vector<sf::Texture*> texVec;
    for (auto texPair : stage->texPathVec) {
        texVec.push_back(&textureHolder->get_texture("bg_" + std::to_string(texPair.first)));
    }
    static sf::Sprite sprite;
    int x = 0;
    int y = 0;
    for (auto& row : stage->levelTexVec) {
        for (auto& val : row) {
            sprite.setTexture(*texVec[val]);
            sprite.setPosition(x*64, y*64);
            renderTex.draw(sprite);
            x++;
        }
        x = 0;
        y++;
    }
    renderTex.display();
}

SceneFore::SceneFore(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageMain*>(&sta);
    textureHolder = &texHolder;
}

void SceneFore::render(float ft) {
    renderTex.clear(sf::Color::Transparent);

    static sf::Sprite playerSprite(textureHolder->get_texture("player_still"), {0,0,64,64});

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

SceneHud::SceneHud(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageHud*>(&sta);
    textureHolder = & texHolder;
    fontVector.push_back(sf::Font());
    fontVector.back().loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render(float ft) {
    static sf::Text timeDisplay("", fontVector[0], 24);

    renderTex.clear(sf::Color::Transparent);

    std::string time = std::to_string(1.f / ft);
    timeDisplay.setString(" FPS: "+time);
    renderTex.draw(timeDisplay);

    renderTex.display();
}
