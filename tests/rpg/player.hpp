#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>
#include <sqee/models/skeleton.hpp>

namespace sqt {

class MainCamera;
class World;

class Player {
public:
    unique_ptr<sq::ModelSkelly> model;

    void test_init();

    void tick();
    void calc(double _accum);

    sq::Application* app = nullptr;
    MainCamera* camera = nullptr;
    World* world = nullptr;

private:
    float rotX = 0.f, rotZ = 0.f;
    float zCam = 0.f;
    vec3 posCrnt, posNext;

    sq::Direction moveDir;
};

}
