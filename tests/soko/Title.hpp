#pragma once

#include <sqee/scenes/Scene.hpp>

namespace sqt {

class SokoApp;

class TitleScene : public sq::Scene {
public:
    TitleScene(SokoApp& _app);

    void update_options();
    void tick(); void render();
    bool handle(sf::Event _event);

private:
    SokoApp& app;
};

}
