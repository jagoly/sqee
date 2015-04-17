#pragma once

#include <sqee/render/cameras.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application& _app);

    sq::Camera camera;

    void update();
    void render(float _ft);
    void resize(uvec2 _size) {}

private:
    vec3 translation = {0.f, 0.f, -4.f};
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
