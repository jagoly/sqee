#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>

#include "entities/model.hpp"
#include "render/camera.hpp"

namespace sqt {

class World;

class Player {
public:
    ent::Model model;

    sq::Skeleton* skelStill;
    sq::Skeleton* skelWalk;

    float rot = 0.f;

    bool moving = false;

    void test_init();
    void attempt_move(sq::Direction _moveDir);

    void tick();
    void calc(double _accum);

    MainCamera* camera = nullptr;
    World* world = nullptr;
    sq::SettingMap* settings = nullptr;

private:
    string layer;

    float zCam = 0;

    sq::Direction moveDir;
    ivec2 moveVal;
    uint moveCntr = 0;
    bool stopped = true;

    ivec2 gridCrnt, gridPrev;
    vec3 posCrnt, posNext;

    vec3 lookCrnt, lookNext;
};

}
