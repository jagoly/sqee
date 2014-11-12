#pragma once

#include <sqee/gl/camera.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

#include "resbank.hpp"
#include "game/settings.hpp"
#include "rendergeneral.hpp"
#include "player.hpp"
#include "maps/world.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application& _app);

    void render(float _ft);
    void resize(glm::uvec2 _size);
    void update();

private:
    Settings settings;

    sq::Camera camera;
    SkyLight skyLight;

    wld::World world;

    Player player;

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
    bool handle(const sf::Event& event);
};

}
