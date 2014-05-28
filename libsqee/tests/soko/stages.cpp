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

void StageMain::update() {
    if (pDir == -1) {
        pDir = get_key_dir();
    } else {
        pMoved = pMoved + 1;

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
            return;
        }
    }
}

StageHud::StageHud() {
    tickRate = 2;
}

void StageHud::update() {
}
