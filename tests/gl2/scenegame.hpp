#pragma once

#include <libsqee/gl/gl.hpp>

#include <libsqee/scenes/scene.hpp>
#include <libsqee/events/handler.hpp>

#include "rendergeneral.hpp"
#include "player.hpp"
#include "maps/level.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    sq::Camera camera;
    SkyLight skyLight;
    Level level;
    Player player;

    void render(float ft);
    void resize(glm::uvec2 _size);
    void update();
private:
    bool updateFramebuffers = true;
    bool updateSkyLight = true;
    bool updateCamera = true;
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
