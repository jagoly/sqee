#pragma once

#include <sqee/render/Camera.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* const _app);

    sq::Camera camera{0};

    void update();
    void render(float _ft);
    void resize(uvec2 _size) {}

private:
    fvec3 translation = {0.f, 0.f, -4.f};
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
