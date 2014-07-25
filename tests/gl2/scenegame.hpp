#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/scene.hpp>
#include <libsqee/handler.hpp>
#include <libsqee/extra/gl.hpp>

#include "rendergeneral.hpp"
#include "maps.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    sqe::Camera camera;
    SkyLight light;
    Ground ground;

    void render(sf::RenderTarget& target, float ft);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
