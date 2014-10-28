#pragma once

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/camera.hpp>
#include <sqee/misc/containers.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application& _app);

    sq::Camera camera;

    void render(float _ft);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
