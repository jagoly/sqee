#pragma once
#include <sqee/forward.hpp>

#include <glm/gtx/hash.hpp>

namespace sqt {

struct Ball {
    using Ptr = unique_ptr<Ball>;
    fvec2 posCrnt, posNext;
    bool pushed, inhole;
    ivec2 position;
};

struct Hole {
    using Ptr = unique_ptr<Hole>;
    bool filled; ivec2 position;
};

struct Wall {
    using Ptr = unique_ptr<Wall>;
    uchar connect, rotate;
    ivec2 position;
};

class Level {
public:
    struct Spec {
        unordered_set<ivec2> ballSet;
        unordered_set<ivec2> holeSet;
        unordered_set<ivec2> wallSet;
        ivec2 minPos = {0, 0};
        ivec2 maxPos = {1, 1};
    };

    Level(const Spec& _spec);
    list<unique_ptr<Ball>> ballList;
    list<unique_ptr<Hole>> holeList;
    list<unique_ptr<Wall>> wallList;
    const ivec2 minPos, maxPos;

    void update();

    Ball* get_Ball(ivec2 _pos);
    Hole* get_Hole(ivec2 _pos);
    Wall* get_Wall(ivec2 _pos);
    bool outside(ivec2 _pos);
};

}
