#pragma once

#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>

namespace sqt {

class PausedScene : public sq::Scene {
public:
    PausedScene(sq::Application* _app);

    void update();
    void render();
    void refresh();
};


class PausedHandler: public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event event);
};

}
