#pragma once
#include "forward.hpp"

#include "events/handler.hpp"
#include "scenes/scene.hpp"

namespace sq {

class SceneConsole : public Scene {
public:
    using sq::Scene::Scene;

    void update();
    void render(float _ft);
    void resize(glm::uvec2 _size);

    void exec();
    void handle_char(char _c);

    void open();
    void close();
    bool active = false;

private:
    uint curPos = 0;
    string inputStr;

    string pretty_print();
};

class HandlerConsole : public Handler {
public:
    using Handler::Handler;
    bool handle(const sf::Event& _event);
};

}
