#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Scene : NonCopyable {
public:
    Scene(Application& _app) : app(_app) {}

    double accum = 0.0;
    uint tickRate = 60;

    virtual void update() {};
    virtual void render(float _ft) {};
    virtual void resize(uvec2 _size) {};

protected:
    Application& app;
};

}
