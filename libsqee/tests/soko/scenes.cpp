#include <iostream>

#include <libsqee/tests/soko/scenes.hpp>

using namespace sqt;

SceneMain::SceneMain(sq::Stage& sta, sq::TextureHolder& texHolder) {
    stage = static_cast<StageMain*>(&sta);
    textureHolder = &texHolder;
    for (auto& path : stage->texPathVec) {
        textureHolder->add_texture("bg_" + std::to_string(path.first), path.second);
    }
    textureHolder->add_texture("wall", "test_soko/wall.png");
    textureHolder->add_texture("hole", "test_soko/hole.png");
    textureHolder->add_texture("rock", "test_soko/rock.png");
    textureHolder->add_texture("goal", "test_soko/goal.png");
}

void SceneMain::render(float ft) {
    renderTex.clear(sf::Color(70, 20, 90));

    static sf::Sprite playerSprite(textureHolder->get_texture("player_still"), {0,0,64,64});

    short int pDir = stage->pDir;
    float portion = stage->pSpeed / stage->tickRate * 64.f;
    float xPos = stage->pX * 64.f;
    float yPos = stage->pY * 64.f;

    if (pDir != -1) {
        float frac = stage->pMoved;
        if (pDir == 0) {
            playerSprite.setTextureRect({0,0,64,64});
            yPos -= (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1.f)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 1) {
            playerSprite.setTextureRect({0,64,64,64});
            xPos += (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1.f)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 2) {
            playerSprite.setTextureRect({64,0,64,64});
            yPos += (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1.f)*portion) * (stage->accum / stage->dt);
        } else
        if (pDir == 3) {
            playerSprite.setTextureRect({64,64,64,64});
            xPos -= (frac*portion) * ((stage->dt - stage->accum) / stage->dt)
                  + ((frac+1.f)*portion) * (stage->accum / stage->dt);
        }
    }

    sf::Vector2f realSize {stage->size.x * 64.f, stage->size.y * 64.f};

    bool xFit       = realSize.x < renderTex.getSize().x;
    bool yFit       = realSize.y < renderTex.getSize().y;
    bool topSide    = yPos < renderTex.getSize().y / 2.f - 32.f;
    bool rightSide  = realSize.x - xPos < renderTex.getSize().x / 2.f + 32.f;
    bool bottomSide = realSize.y - yPos < renderTex.getSize().y / 2.f + 32.f;
    bool leftSide   = xPos < renderTex.getSize().x / 2.f - 32.f;

    float xOff;
    float yOff;

    if (xFit) {
        xOff = (renderTex.getSize().x - realSize.x) / 2.f;
    } else if (leftSide) {
        xOff = 0.f;
    } else if (rightSide) {
        xOff = renderTex.getSize().x - realSize.x;
    } else {
        xOff = 0.f - xPos + renderTex.getSize().x / 2.f - 32.f;
    }

    if (yFit) {
        yOff = (renderTex.getSize().y - realSize.y) / 2.f;
    } else if (topSide) {
        yOff = 0.f;
    } else if (bottomSide) {
        yOff = renderTex.getSize().y - realSize.y;
    } else {
        yOff = 0.f - yPos + renderTex.getSize().y / 2.f - 32.f;
    }

    static sf::Sprite sprite;
    static std::vector<sf::Texture*> texVec;
    unsigned short int x = 0;
    unsigned short int y = 0;

    texVec.clear();
    for (auto texPair : stage->texPathVec) {
        texVec.push_back(&textureHolder->get_texture("bg_" + std::to_string(texPair.first)));
    }
    for (auto& row : stage->levelTexVec) {
        for (auto& val : row) {
            sprite.setTexture(*texVec[val]);
            sprite.setPosition(x * 64.f + xOff, y * 64.f + yOff);
            renderTex.draw(sprite);
            x++;
        }
        x = 0;
        y++;
    }

    static std::vector<sf::Texture*> texVecObj {
        nullptr,
        &textureHolder->get_texture("wall"),
        &textureHolder->get_texture("hole"),
        &textureHolder->get_texture("rock"),
        &textureHolder->get_texture("goal"),
        nullptr
    };

    x = 0;
    y = 0;
    for (auto& row : stage->levelObjVec) {
        for (auto& val : row) {
            if (val && val != 5) {
                sprite.setTexture(*texVecObj[val]);
                sprite.setPosition(x * 64.f + xOff, y * 64.f + yOff);
                renderTex.draw(sprite);
            }
            x++;
        }
        x = 0;
        y++;
    }

    playerSprite.setPosition(xPos + xOff, yPos + yOff);
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
    renderTex.clear(sf::Color::Transparent);

    static float fps = 60.f;
    fps = fps * 0.9f + 1.f / ft * 0.1f;

    static sf::Text textDisplay("", fontVector[0], 24);

    textDisplay.setString(" FPS: " + std::to_string(fps) +
                          "\n X: " + std::to_string(stage->pX) +
                          "\n Y: " + std::to_string(stage->pY));
    renderTex.draw(textDisplay);

    renderTex.display();
}
