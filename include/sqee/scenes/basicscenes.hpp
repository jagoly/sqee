#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/scene.hpp>

namespace sq {

class SceneFPS : public Scene {
public:
    using sq::Scene::Scene;

    void update();
    void render(float _ft);
    void resize(uvec2 _size);
};


class SceneMouse : public Scene {
public:
    SceneMouse();

    void update();
    void render(float _ft);
    void resize(uvec2 _size);
};

}
