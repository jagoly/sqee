#pragma once

#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>

namespace sqt {

class TitleScene : public sq::Scene {
public:
    TitleScene(sq::Application* _app);

    void update();
    void render();
    void refresh();
};


class TitleHandler: public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(sf::Event event);
};

}
