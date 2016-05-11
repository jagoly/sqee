#pragma once

#include <sqee/scenes/Scene.hpp>

namespace sqt {

class TitleScene : public sq::Scene {
public:
    TitleScene(sq::Application* _app);

    void update();
    void render();
    void refresh();

    bool handle(sf::Event _event);
};

}
