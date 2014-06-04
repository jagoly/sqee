#include <iostream>

#include "scenemain.hpp"
#include "helpers.hpp"

using namespace sqt;

SceneMain::SceneMain(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 32;
    dt = 0.03125f;

    pDir = -1;
    pFace = 0;
    pX = 0;
    pY = 0;
    pSpeed = 4.f;
    pMoved = 0;
    pushing = {-1, -1};

    app->texHolder.add_texture("wall", "res/wall.png");
    app->texHolder.add_texture("hole", "res/hole.png");
    app->texHolder.add_texture("rock", "res/rock.png");
    app->texHolder.add_texture("goal", "res/goal.png");
}

void SceneMain::update() {
    pFace = -1;
    if (pDir == -1) {
        short int temp = get_key_dir();
        if (temp != -1) pFace = temp;
        else return;
        if (check_go(temp, pX, pY, size, levelObjVec))
            pDir = temp;
        else return;

        sf::Vector2i c;
        if      (pDir == 0) c = {pX, pY-1};
        else if (pDir == 1) c = {pX+1, pY};
        else if (pDir == 2) c = {pX, pY+1};
        else                c = {pX-1, pY};

        if (levelObjVec[c.y][c.x] == 3) pushing = c;

    }

    pMoved += 1;
    if (pMoved == int(tickRate / pSpeed)) {
        sf::Vector2i cOne;
        if      (pDir == 0) cOne = {pX, pY-1};
        else if (pDir == 1) cOne = {pX+1, pY};
        else if (pDir == 2) cOne = {pX, pY+1};
        else                cOne = {pX-1, pY};

        unsigned short int oOne = levelObjVec[cOne.y][cOne.x];

        if (oOne == 3) {
            levelObjVec[cOne.y][cOne.x] = 0;

            sf::Vector2i cTwo;
            if      (pDir == 0) cTwo = {pX, pY-2};
            else if (pDir == 1) cTwo = {pX+2, pY};
            else if (pDir == 2) cTwo = {pX, pY+2};
            else                cTwo = {pX-2, pY};

            unsigned short int oTwo = levelObjVec[cTwo.y][cTwo.x];

            if (oTwo == 0) levelObjVec[cTwo.y][cTwo.x] = 3;
            else           levelObjVec[cTwo.y][cTwo.x] = 0;
        }

        if      (pDir == 0) pY -= 1;
        else if (pDir == 1) pX += 1;
        else if (pDir == 2) pY += 1;
        else                pX -= 1;

        pMoved = 0;
        pDir = -1;
        pushing = {-1, -1};
        pFace = get_key_dir();
        if (pFace != -1 && check_go(pFace, pX, pY, size, levelObjVec)) {
            pDir = pFace;

            sf::Vector2i c;
            if      (pDir == 0) c = {pX, pY-1};
            else if (pDir == 1) c = {pX+1, pY};
            else if (pDir == 2) c = {pX, pY+1};
            else                c = {pX-1, pY};

            if (levelObjVec[c.y][c.x] == 3) pushing = c;
        }
    }
}

void SceneMain::load_level(Level& level) {
    size = {level.width, level.height};
    for (auto& path : level.paths) {
        texPathVec.push_back(path);
    }
    levelTexVec = level.textures;
    levelObjVec = level.objects;

    for (auto& path : texPathVec) {
        app->texHolder.add_texture("bg_" + std::to_string(path.first), path.second);
    }
}

void SceneMain::render(sf::RenderTarget& target, float ft) {
    target.clear(sf::Color(70, 20, 90));
    static sf::Sprite playerSprite(app->texHolder.get_texture("player_still"), {0,0,64,64});

    float portion = pSpeed / tickRate * 64.f;
    float xPos = pX * 64.f;
    float yPos = pY * 64.f;

    if (pFace != -1) {
        if      (pFace == 0) playerSprite.setTextureRect({0,0,64,64});
        else if (pFace == 1) playerSprite.setTextureRect({0,64,64,64});
        else if (pFace == 2) playerSprite.setTextureRect({64,0,64,64});
        else                 playerSprite.setTextureRect({64,64,64,64});
    }

    int xPor = 0;
    int yPor = 0;
    if (pDir != -1) {
        float frac = pMoved;
        if (pDir == 0) {
            yPor -= (frac*portion) * ((dt - accum) / dt)
                  + ((frac+1.f)*portion) * (accum / dt);
        } else
        if (pDir == 1) {
            xPor += (frac*portion) * ((dt - accum) / dt)
                  + ((frac+1.f)*portion) * (accum / dt);
        } else
        if (pDir == 2) {
            yPor += (frac*portion) * ((dt - accum) / dt)
                  + ((frac+1.f)*portion) * (accum / dt);
        } else
        if (pDir == 3) {
            xPor -= (frac*portion) * ((dt - accum) / dt)
                  + ((frac+1.f)*portion) * (accum / dt);
        }
    }

    sf::Vector2f realSize {size.x * 64.f, size.y * 64.f};

    bool xFit       = realSize.x < target.getSize().x;
    bool yFit       = realSize.y < target.getSize().y;
    bool topSide    = yPos + yPor < target.getSize().y / 2.f - 32.f;
    bool rightSide  = realSize.x - (xPos + xPor) < target.getSize().x / 2.f + 32.f;
    bool bottomSide = realSize.y - (yPos + yPor) < target.getSize().y / 2.f + 32.f;
    bool leftSide   = xPos + xPor < target.getSize().x / 2.f - 32.f;

    float xOff;
    float yOff;

    if (xFit)           xOff = (target.getSize().x - realSize.x) / 2.f;
    else if (leftSide)  xOff = 0.f;
    else if (rightSide) xOff = target.getSize().x - realSize.x;
    else                xOff = 0.f - (xPos + xPor) + target.getSize().x / 2.f - 32.f;

    if (yFit)            yOff = (target.getSize().y - realSize.y) / 2.f;
    else if (topSide)    yOff = 0.f;
    else if (bottomSide) yOff = target.getSize().y - realSize.y;
    else                 yOff = 0.f - (yPos + yPor) + target.getSize().y / 2.f - 32.f;

    static sf::Sprite sprite;
    static std::vector<sf::Texture*> texVec;
    unsigned short int x = 0;
    unsigned short int y = 0;

    texVec.clear();
    for (auto texPair : texPathVec) {
        texVec.push_back(&app->texHolder.get_texture("bg_" + std::to_string(texPair.first)));
    }
    for (auto& row : levelTexVec) {
        for (auto& val : row) {
            sprite.setTexture(*texVec[val]);
            sprite.setPosition(x * 64.f + xOff, y * 64.f + yOff);
            target.draw(sprite);
            x++;
        }
        x = 0;
        y++;
    }

    static std::vector<sf::Texture*> texVecObj {
        nullptr,
        &app->texHolder.get_texture("wall"),
        &app->texHolder.get_texture("hole"),
        &app->texHolder.get_texture("rock"),
        &app->texHolder.get_texture("goal"),
        nullptr
    };

    x = 0;
    y = 0;
    for (auto& row : levelObjVec) {
        for (auto& val : row) {
            if (val && val != 5) {
                sprite.setTexture(*texVecObj[val]);
                if (x != pushing.x || y != pushing.y) {
                    sprite.setPosition(x * 64.f + xOff, y * 64.f + yOff);
                    target.draw(sprite);
                }
            }
            x++;
        }
        x = 0;
        y++;
    }

    if (pushing.x != -1) {
        sprite.setTexture(*texVecObj[3]);
        sprite.setPosition(pushing.x * 64.f + xOff + xPor,
                           pushing.y * 64.f + yOff + yPor);
        target.draw(sprite);
    }

    playerSprite.setPosition(xPos + xPor + xOff, yPos + yPor + yOff);
    target.draw(playerSprite);
}
