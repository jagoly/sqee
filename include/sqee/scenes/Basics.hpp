#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>

namespace sq {

class BasicFPSScene : public Scene {
public:
    using sq::Scene::Scene;
    void render(float _ft);

private:
    float ft = 1.f / 60.f;
};

}
