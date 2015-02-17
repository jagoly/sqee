#pragma once
#include "forward.hpp"

#include <memory>

namespace sq {

class Application;

class Scene : NonCopyable {
public:
    typedef std::unique_ptr<Scene> Ptr;

    Scene(Application& _app)
        : app(_app) {}

    double accum = 0.d;
    uint tickRate = 60;

    virtual void update() =0;
    virtual void render(float _ft) =0;
    virtual void resize(glm::uvec2 _size) =0;

protected:
    Application& app;
};

}
