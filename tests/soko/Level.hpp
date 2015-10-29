#pragma once

#include <list>
#include <set>

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

struct Ball {
    using Ptr = unique_ptr<Ball>;
    Vec2F posCrnt, posNext;
    bool pushed, inhole;
    Vec2I position;
};

struct Hole {
    using Ptr = unique_ptr<Hole>;
    bool filled; Vec2I position;
};

struct Wall {
    using Ptr = unique_ptr<Wall>;
    uchar connect, rotate;
    Vec2I position;
};

class Level {
public:
    struct Spec {
        std::set<Vec2I> ballSet;
        std::set<Vec2I> holeSet;
        std::set<Vec2I> wallSet;
        Vec2I minPos = {0, 0};
        Vec2I maxPos = {1, 1};
    };

    Level(const Spec& _spec);
    std::list<unique_ptr<Ball>> ballList;
    std::list<unique_ptr<Hole>> holeList;
    std::list<unique_ptr<Wall>> wallList;
    const Vec2I minPos, maxPos;

    void update();

    Ball* get_Ball(Vec2I _pos);
    Hole* get_Hole(Vec2I _pos);
    Wall* get_Wall(Vec2I _pos);
    bool outside(Vec2I _pos);
};

}
