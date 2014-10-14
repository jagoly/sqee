#pragma once

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/gl/camera.hpp>
#include <libsqee/misc/containers.hpp>
#include <libsqee/scenes/scene.hpp>
#include <libsqee/events/handler.hpp>

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    sq::Camera camera;

    void render(float _ft);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& _event);
};

}
