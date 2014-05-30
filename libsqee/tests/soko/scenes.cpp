#include <iostream>

#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

SceneMain::SceneMain(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageMain*>(&sta);
    textureHolder = &texHolder;
    for (auto& path : stage->texPathVec) {
        textureHolder->add_texture("bg_" + std::to_string(path.first), path.second);
    }
}

void SceneMain::render(float ft) {
    renderTex.clear(sf::Color(70, 20, 90));

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

    sf::Vector2u realSize {stage->size.x * 64, stage->size.y * 64};

    bool rightSide  = realSize.x - posX < renderTex.getSize().x / 2 + 32.f;
    bool bottomSide = realSize.y - posY < renderTex.getSize().y / 2 + 32.f;

    float xOffset = renderTex.getSize().x / 2.f - 32.f;
    float yOffset = renderTex.getSize().y / 2.f - 32.f;

    std::vector<sf::Texture*> texVec;
    for (auto texPair : stage->texPathVec) {
        texVec.push_back(&textureHolder->get_texture("bg_" + std::to_string(texPair.first)));
    }

    static sf::Sprite sprite;
    int x = 0;
    int y = 0;

    for (auto& row : stage->levelTexVec) {
        for (auto& val : row) {
            sprite.setTexture(*texVec[val]);
            if (rightSide && bottomSide) {
                sprite.setPosition(renderTex.getSize().x - (realSize.x - x * 64),
                                   renderTex.getSize().y - (realSize.y - y * 64));
            } else
            if (rightSide) {
                sprite.setPosition(renderTex.getSize().x - (realSize.x - x * 64),
                                   y * 64 - posY + yOffset);
            } else
            if (bottomSide) {
                sprite.setPosition(x * 64 - posX + xOffset,
                                   renderTex.getSize().y - (realSize.y - y * 64));
            } else {
                sprite.setPosition(x * 64 - posX + xOffset,
                                   y * 64 - posY + yOffset);
            }
            renderTex.draw(sprite);
            x++;
        }
        x = 0;
        y++;
    }

    if (!rightSide) x = xOffset;
    else x = renderTex.getSize().x - (realSize.x - posX);

    if (!bottomSide) y = yOffset;
    else y = renderTex.getSize().y - (realSize.y - posY);

    playerSprite.setPosition(x, y);
    renderTex.draw(playerSprite);

    renderTex.display();
}

SceneHud::SceneHud(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageHud*>(&sta);
    textureHolder = &texHolder;
    fontVector.push_back(sf::Font());
    fontVector.back().loadFromFile("test_soko/DroidSans.ttf");
}

void SceneHud::render(float ft) {
    static sf::Text textDisplay("", fontVector[0], 24);

    renderTex.clear(sf::Color::Transparent);

    static float fps = 60.f;
    fps = fps * 0.9f + 1.f / ft * 0.1f;

    textDisplay.setString(" FPS: " + std::to_string(fps) +
                          "\n X: " + std::to_string(stage->pX) +
                          "\n Y: " + std::to_string(stage->pY));
    renderTex.draw(textDisplay);

    renderTex.display();
}
