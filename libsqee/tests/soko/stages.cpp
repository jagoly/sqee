#include <iostream>

#include <libsqee/tests/soko/stages.hpp>
#include <libsqee/tests/soko/helpers.hpp>

using namespace sqt;

StageMain::StageMain() {
    tickRate = 32;
    dt = 0.03125f;

    pDir = -1;
    pFace = 0;
    pX = 0;
    pY = 0;
    pSpeed = 4.f;
    pMoved = 0;
    pushing = {-1, -1};
}

void StageMain::update() {
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


void StageMain::load_level(Level* level) {
    size = {level->width, level->height};
    for (auto& path : level->paths) {
        texPathVec.push_back(path);
    }
    levelTexVec = level->textures;
    levelObjVec = level->objects;
}

StageHud::StageHud() {
    tickRate = 16;
    pX = 0;
    pY = 0;
}

void StageHud::update() {
    pX = static_cast<StageMain*>(&application->get_stage("main"))->pX;
    pY = static_cast<StageMain*>(&application->get_stage("main"))->pY;
}
