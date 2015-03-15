#pragma once
#include "forward.hpp"

namespace sq {

class Application;

class Scene : NonCopyable {
public:
    using Ptr = unique_ptr<Scene>;

    Scene(Application& _app) : app(_app) {}

    double accum = 0.0;
    uint tickRate = 60;

    virtual void update() = 0;
    virtual void render(float _ft) = 0;
    virtual void resize(uvec2 _size) = 0;

protected:
    Application& app;
};

}
