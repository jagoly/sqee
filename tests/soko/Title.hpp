#pragma once

#include <sqee/scenes/Scene.hpp>

namespace sqt {

class TitleScene : public sq::Scene {
public:
    TitleScene(sq::Application* _app);

    void configure();
    void tick(), render();

    bool handle(sf::Event _event);
};

}
