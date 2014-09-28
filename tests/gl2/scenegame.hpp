#pragma once

#include <libsqee/gl/gl.hpp>
#include <libsqee/resources/resholder.hpp>

#include <libsqee/scenes/scene.hpp>
#include <libsqee/events/handler.hpp>

#include "rendergeneral.hpp"
#include "player.hpp"
#include "maps/level.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    void render(float _ft);
    void resize(glm::uvec2 _size);
    void update();
private:
    sq::TexHolder texHolder;

    Settings settings;

    sq::Camera camera;
    Level level;
    SkyLight skyLight;

    Player player;
    ushort arrowPriority[4];

    int tickTock = 0;
    bool pendDir = true;

    float swingA, swingB;

    bool modSettings = true;
    bool updateFramebuffers = true;
    bool updateSkyLight = true;
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
