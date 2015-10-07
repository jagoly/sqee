#pragma once
#include <sqee/forward.hpp>
#include <unordered_set>
#include <list>

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
        std::unordered_set<ivec2> ballSet;
        std::unordered_set<ivec2> holeSet;
        std::unordered_set<ivec2> wallSet;
        ivec2 minPos = {0, 0};
        ivec2 maxPos = {1, 1};
    };

    Level(const Spec& _spec);
    std::list<unique_ptr<Ball>> ballList;
    std::list<unique_ptr<Hole>> holeList;
    std::list<unique_ptr<Wall>> wallList;
    const ivec2 minPos, maxPos;

    void update();

    Ball* get_Ball(ivec2 _pos);
    Hole* get_Hole(ivec2 _pos);
    Wall* get_Wall(ivec2 _pos);
    bool outside(ivec2 _pos);
};

}
