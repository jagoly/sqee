#include <iostream>

#include "Level.hpp"

using namespace sqt;
namespace maths = sq::maths;

Level::Level(const Spec& _spec) : minPos(_spec.minPos), maxPos(_spec.maxPos) {
    for (Vec2I pos : _spec.ballSet) ballList.emplace_front(new Ball{Vec2F(pos), Vec2F(pos), false, false, pos});
    for (Vec2I pos : _spec.holeSet) holeList.emplace_front(new Hole{false, pos});

    for (Vec2I pos : _spec.wallSet) {
        bool xNeg, xPos, yNeg, yPos;
        uchar connect = 6u, rotate = 4u;

        if (pos.x == minPos.x) xNeg = false;
        else xNeg = _spec.wallSet.count({pos.x-1, pos.y});
        if (pos.x == maxPos.x-1) xPos = false;
        else xPos = _spec.wallSet.count({pos.x+1, pos.y});
        if (pos.y == minPos.y) yNeg = false;
        else yNeg = _spec.wallSet.count({pos.x, pos.y-1});
        if (pos.y == maxPos.y-1) yPos = false;
        else yPos = _spec.wallSet.count({pos.x, pos.y+1});

        if (!yNeg && !yPos && !xNeg && !xPos) connect = 0u, rotate = 0u;
        if (!yNeg && !yPos && !xNeg &&  xPos) connect = 2u, rotate = 3u;
        if (!yNeg && !yPos &&  xNeg && !xPos) connect = 2u, rotate = 1u;
        if (!yNeg && !yPos &&  xNeg &&  xPos) connect = 3u, rotate = 1u;
        if (!yNeg &&  yPos && !xNeg && !xPos) connect = 2u, rotate = 2u;
        if (!yNeg &&  yPos && !xNeg &&  xPos) connect = 4u, rotate = 0u;
        if (!yNeg &&  yPos &&  xNeg && !xPos) connect = 4u, rotate = 3u;
        if (!yNeg &&  yPos &&  xNeg &&  xPos) connect = 5u, rotate = 0u;
        if ( yNeg && !yPos && !xNeg && !xPos) connect = 2u, rotate = 0u;
        if ( yNeg && !yPos && !xNeg &&  xPos) connect = 4u, rotate = 1u;
        if ( yNeg && !yPos &&  xNeg && !xPos) connect = 4u, rotate = 2u;
        if ( yNeg && !yPos &&  xNeg &&  xPos) connect = 5u, rotate = 2u;
        if ( yNeg &&  yPos && !xNeg && !xPos) connect = 3u, rotate = 0u;
        if ( yNeg &&  yPos && !xNeg &&  xPos) connect = 5u, rotate = 1u;
        if ( yNeg &&  yPos &&  xNeg && !xPos) connect = 5u, rotate = 3u;
        if ( yNeg &&  yPos &&  xNeg &&  xPos) connect = 1u, rotate = 0u;

        wallList.emplace_front(new Wall{connect, rotate, pos});
    }
}


void Level::update() {
    for (auto iter = ballList.begin(); iter != ballList.end();) {
        Ball& ball = *iter->get();
        Hole* hole = get_Hole(ball.position);

        if (ball.inhole == true) {
            iter = ballList.erase(iter);
            hole->filled = true;
        } else {
            bool inhole = !ball.pushed && hole != nullptr && !hole->filled;
            Vec2F posCrnt = ball.posNext, posNext = ball.posNext;
            Vec2I position = ball.position;

            if (ball.pushed == false) posNext = Vec2F(position);
            else posNext = maths::mix(posCrnt, Vec2F(position), 0.5f);
            iter->reset(new Ball{posCrnt, posNext, false, inhole, position});

            std::advance(iter, 1u);
        }
    }
}


Ball* Level::get_Ball(Vec2I _pos) {
    for (Ball::Ptr& ball : ballList) {
        if (ball->position == _pos) return ball.get();
    } return nullptr;
}


Hole* Level::get_Hole(Vec2I _pos) {
    for (Hole::Ptr& hole : holeList) {
        if (hole->position == _pos) return hole.get();
    } return nullptr;
}


Wall* Level::get_Wall(Vec2I _pos) {
    for (Wall::Ptr& wall : wallList) {
        if (wall->position == _pos) return wall.get();
    } return nullptr;
}


bool Level::outside(Vec2I _pos) {
    return _pos.x < minPos.x || _pos.y < minPos.y ||
           _pos.x >= maxPos.x || _pos.y >= maxPos.y;
}
