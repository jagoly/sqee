#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/scene.hpp>
#include <libsqee/handler.hpp>
#include <libsqee/gl/gl.hpp>

#include "rendergeneral.hpp"
#include "player.hpp"
#include "maps.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    sq::Camera camera;
    SkyLight light;
    LevelMap levelMap;
    Player player;

    void render(sf::RenderTarget& target, float ft);
    void update();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
