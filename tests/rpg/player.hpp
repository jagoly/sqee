#pragma once
#include <sqee/forward.hpp>

#include "entities/model.hpp"

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


    sq::LookatCamera* camera = nullptr;
    World* world = nullptr;

private:
    string layer;

    sq::Direction moveDir;
    glm::ivec2 moveVal;
    uint moveCntr = 0;
    bool stopped = true;

    glm::ivec2 gridCrnt, gridPrev;
    glm::vec3 posCrnt, posNext;
    glm::vec2 camOffsCrnt, camOffsNext, camOffsTrgt;
};

}
