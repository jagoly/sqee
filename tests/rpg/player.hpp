#pragma once

#include <SFML/System.hpp>

#include <sqee/gl/camera.hpp>

#include "resbank.hpp"
#include "maps/world.hpp"
#include "entities/model.hpp"

namespace sqt {

class Player {
public:
    ent::Model model;

    sq::Skeleton* skelStill;
    sq::Skeleton* skelWalk;

    float rot = 0.f;

    bool moving = false;

    void test_init();

    void tick(char _moveDir, wld::World& _world);
    void calc(double _accum, sq::Camera& _camera);

private:
    string layer;
    glm::uvec2 gridPos;

    glm::vec3 posCrnt;
    glm::vec3 posNext;
};

}
