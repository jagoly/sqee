#pragma once

#include <memory>

#include <glm/glm.hpp>

namespace sq {

class Application;

class Scene {
public:
    typedef std::unique_ptr<Scene> Ptr;

    Scene(Application* _app);

    double accum = 0.d;
    int tickRate = 60;
    double dt = 1.d/60.d;

    virtual void update();
    virtual void render(float ft);
    virtual void resize(glm::uvec2 _size);

protected:
    Application* app;
};

}

#include <app/application.hpp>
