#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include <gl/textures.hpp>

namespace sq {

class Application;

class Scene {
public:
    Scene(Application* _app);

    double accum;
    int tickRate = 60;
    double dt = 1.d/60.d;

    virtual void resize(glm::uvec2 _size);
    virtual void update();
    virtual void render(float ft);
protected:
    Application* app;

    template<typename T>
    T interpolate(T a, T b);
};

typedef std::shared_ptr<Scene> ScenePtr;

}
