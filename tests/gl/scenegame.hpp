#pragma once

#include <sqee/render/cameras.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/handlers/handler.hpp>

namespace sqt {

class GlApp;

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application* const _app);

    sq::Camera camera;

    void update();
    void render(float _ft);
    void resize(glm::uvec2 _size);

private:
    fvec3 translation = {0.f, 0.f, -4.f};
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
