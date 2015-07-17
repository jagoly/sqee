#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>

namespace sqt {

class PausedScene : public sq::Scene {
public:
    PausedScene(sq::Application* _app);

    void update();
    void render(float _ft);
    void update_settings();
};


class PausedHandler: public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
