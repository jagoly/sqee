#pragma once

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/camera.hpp>
#include <sqee/misc/containers.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

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
    obj::MeshHolder meshH;
    obj::SkinHolder skinH;
    sq::TexHolder texH;
    AdvMeshHolder advMeshH;
    AdvSkinHolder advSkinH;

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
