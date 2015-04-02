#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>
#include <sqee/scenes/scene.hpp>

namespace sq {

class SceneConsole : public Scene {
public:
    using sq::Scene::Scene;

    void update();
    void render(float _ft);
    void resize(uvec2 _size);

    void exec();
    void handle_character(char _char);
    void handle_action(int _action);

    void toggle_active();

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
