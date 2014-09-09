#pragma once

#include <libsqee/gl/gl.hpp>
#include <libsqee/resources/texholder.hpp>

#include <libsqee/scenes/scene.hpp>
#include <libsqee/events/handler.hpp>

#include "rendergeneral.hpp"
#include "player.hpp"
#include "maps/level.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* _app);

    void render(float ft);
    void resize(glm::uvec2 _size);
    void update();
private:
    sq::TexHolderBasic texHolder;

    sq::Camera camera;
    Level level;
    SkyLight skyLight;

    Player player;
    ushort arrowPriority[4];

    bool updateFramebuffers = true;
    bool updateSkyLight = true;
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event& event);
};

}
