#include "Level.hpp"

using namespace sqt;
namespace maths = sq::maths;

//============================================================================//

Level::Level(const Specification& spec) : minPos(spec.minPos), maxPos(spec.maxPos)
{
    for (const auto& pos : spec.ballSet) ballList.push_front({ Vec2F(pos), Vec2F(pos), false, false, pos });
    for (const auto& pos : spec.holeSet) holeList.push_front({ false, pos });
    for (const auto& pos : spec.wallSet) wallList.push_front({ pos });
}

//============================================================================//

void Level::update()
{
    for (auto iter = ballList.begin(); iter != ballList.end();)
    {
        Ball& ball = *iter;
        Hole* hole = get_hole(ball.position);

        if (ball.complete == true)
        {
            iter = ballList.erase(iter);
            hole->complete = true;
        }
        else
        {
            bool inHole = !ball.pushed && hole != nullptr && !hole->complete;
            Vec2F posCrnt = ball.posNext, posNext = ball.posNext;
            Vec2I position = ball.position;

            if (ball.pushed == false) posNext = Vec2F(position);
            else posNext = maths::mix(posCrnt, Vec2F(position), 0.5f);
            ball = { posCrnt, posNext, false, inHole, position };

            std::advance(iter, 1u);
        }
    }
}

//============================================================================//

Ball* Level::get_ball(Vec2I pos)
{
    for (Ball& ball : ballList)
        if (ball.position == pos)
            return &ball;

    return nullptr;
}

Hole* Level::get_hole(Vec2I pos)
{
    for (Hole& hole : holeList)
        if (hole.position == pos)
            return &hole;

    return nullptr;
}

Wall* Level::get_wall(Vec2I pos)
{
    for (Wall& wall : wallList)
        if (wall.position == pos)
            return &wall;

    return nullptr;
}

//============================================================================//

bool Level::outside(Vec2I pos)
{
    return pos.x < minPos.x || pos.y < minPos.y || pos.x >= maxPos.x || pos.y >= maxPos.y;
}
