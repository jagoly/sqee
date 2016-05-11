#pragma once

#include <sqee/scenes/Scene.hpp>

namespace sqt {

class PausedScene : public sq::Scene {
public:
    PausedScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

    bool handle(sf::Event _event);
};

}
