#pragma once

#include <sqee/gl/cameras.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

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
    sq::Camera camera;
    wld::World world;
    Player player;

    bool updateSettings = true;
    bool updateFramebuffers = true;
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
