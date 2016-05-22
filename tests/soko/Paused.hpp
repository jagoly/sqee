#pragma once

#include <sqee/scenes/Scene.hpp>

namespace sqt {

class PausedScene : public sq::Scene {
public:
    PausedScene(sq::Application* _app);

    void configure();
    void tick(), render();

    bool handle(sf::Event _event);
};

}
