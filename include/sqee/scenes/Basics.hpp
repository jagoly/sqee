#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>

namespace sq {

class SceneFPS : public Scene {
public:
    using sq::Scene::Scene;
    void render(float _ft);
};

}
