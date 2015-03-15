#pragma once
#include "forward.hpp"

#include "scenes/scene.hpp"

namespace sq {

class SceneFPS : public Scene {
public:
    using sq::Scene::Scene;

    void update();
    void render(float _ft);
    void resize(uvec2 _size);
};

}
