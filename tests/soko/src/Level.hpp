#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <list>
#include <set>

using namespace sq::coretypes;

namespace sqt {

//============================================================================//

struct Ball
{
    Vec2F posCrnt, posNext;
    bool pushed, complete;
    Vec2I position;
};

struct Hole
{
    bool complete;
    Vec2I position;
};

struct Wall
{
    Vec2I position;
};

//============================================================================//

class Level : sq::NonCopyable
{
public: //====================================================//

    struct Specification
    {
        std::set<Vec2I> ballSet;
        std::set<Vec2I> holeSet;
        std::set<Vec2I> wallSet;
        Vec2I minPos = {0, 0};
        Vec2I maxPos = {1, 1};
    };

    //--------------------------------------------------------//

    Level(const Specification& spec);

    std::list<Ball> ballList;
    std::list<Hole> holeList;
    std::list<Wall> wallList;

    const Vec2I minPos, maxPos;

    //--------------------------------------------------------//

    void update();

    Ball* get_ball(Vec2I pos);
    Hole* get_hole(Vec2I pos);
    Wall* get_wall(Vec2I pos);

    bool outside(Vec2I pos);
};

}
