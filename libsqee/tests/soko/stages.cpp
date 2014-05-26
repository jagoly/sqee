#include <iostream>

#include <libsqee/tests/soko/stages.hpp>
#include <libsqee/tests/soko/helpers.hpp>

using namespace sqt;
using boost::get;

StageMain::StageMain() {
    tickRate = 32;
    dt = 0.03125f;
    stateMap.emplace(std::pair<std::string, int>("pDir", -1));
    stateMap.emplace(std::pair<std::string, int>("pX", 4));
    stateMap.emplace(std::pair<std::string, int>("pY", 4));
    stateMap.emplace(std::pair<std::string, float>("pSpeed", 4.f));
    stateMap.emplace(std::pair<std::string, int>("pMoved", 0));
}

void StageMain::update() {
    if (get<int>(stateMap["pDir"]) == -1) {
        stateMap["pDir"] = get_key_dir();
    } else {
        int pDir = get<int>(stateMap["pDir"]);
        stateMap["pMoved"] = get<int>(stateMap["pMoved"]) + 1;

        if (get<int>(stateMap["pMoved"]) == int(tickRate / get<float>(stateMap["pSpeed"]))) {
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
            stateMap["pDir"] = get_key_dir();
            return;
        }
    }
}

StageHud::StageHud() {
    tickRate = 2;
}

void StageHud::update() {
}
