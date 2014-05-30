#include <iostream>

#include <libsqee/tests/soko/stages.hpp>
#include <libsqee/tests/soko/helpers.hpp>

using namespace sqt;

StageMain::StageMain() {
    tickRate = 32;
    dt = 0.03125f;

    pDir = -1;
    pX = 4;
    pY = 4;
    pSpeed = 4.f;
    pMoved = 0;
}

#define set_return {pDir = -1; return;}
void StageMain::update() {
    bool canGo = true;
    short int gDir = pDir;

    if (pDir == -1) {
        gDir = get_key_dir();
    }

    sf::Vector2i cOne;
    unsigned short int oOne;

    if (gDir == 0) {
        cOne = {pY-1, pX};
        oOne = levelObjVec[pY-1][pX];
    } else
    if (gDir == 1) {
        cOne = {pY, pX+1};
        oOne = levelObjVec[pY][pX+1];
    } else
    if (gDir == 2) {
        cOne = {pY+1, pX};
        oOne = levelObjVec[pY+1][pX];
    } else {
        cOne = {pY, pX-1};
        oOne = levelObjVec[pY][pX-1];
    }

    if ((pDir == 0 && pY == 0) ||
        (pDir == 1 && pX == size.x-1) ||
        (pDir == 2 && pY == size.y-1) ||
        (pDir == 3 && pX == 0)) set_return


    if (oOne == 1 || oOne == 2) set_return

    if (oOne == 3) {
        sf::Vector2i cTwo;
        unsigned short int oTwo;

        if (pDir == 0) {
            cTwo = {pY-2, pX};
            oTwo = levelObjVec[pY-2][pX];
        } else
        if (pDir == 1) {
            cTwo = {pY, pX+2};
            oTwo = levelObjVec[pY][pX+2];
        } else
        if (pDir == 2) {
            cTwo = {pY+2, pX};
            oTwo = levelObjVec[pY+2][pX];
        } else {
            cTwo = {pY, pX-2};
            oTwo = levelObjVec[pY][pX-2];
        }

        if (oTwo == 1 || oTwo == 3 || oTwo == 4 ||
            (pDir == 0 && pY == 1) ||
            (pDir == 1 && pX == size.x-2) ||
            (pDir == 2 && pY == size.y-2) ||
            (pDir == 3 && pX == 1)) set_return
        pushing = cTwo;
    }

    pMoved += 1;
    if (pMoved == int(tickRate / pSpeed)) {
        if (pDir == 0) {
            pY -= 1;
        } else
        if (pDir == 1) {
            pX += 1;
        } else
        if (pDir == 2) {
            pY += 1;
        } else
        if (pDir == 3) {
            pX -= 1;
        }
        pMoved = 0;

        pDir = get_key_dir();
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
