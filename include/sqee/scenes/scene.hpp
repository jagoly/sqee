#pragma once

#include <gl/gl.hpp>

namespace sq {

class Application;

class Scene {
public:
    typedef unique_ptr<Scene> Ptr;

    Scene(Application& _app)
        : app(_app) {}

    double accum = 0.d;
    uint tickRate = 60;

    virtual void update() {}
    virtual void render(float _ft) {}
    virtual void resize(glm::uvec2 _size) {}

protected:
    Application& app;
};

}

#include <app/application.hpp>
