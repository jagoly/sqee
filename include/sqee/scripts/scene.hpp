#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/scene.hpp>

namespace sq {

class SceneConsole : public Scene {
public:
    SceneConsole(Application& _app);

    enum class Action { Up, Down, Left, Right,
                        Backspace, Delete, Home, End,
                        Return, ShiftReturn };

    void update();
    void render(float _ft);
    void resize(uvec2 _size);

    void exec();
    void handle_character(char _char);
    void handle_action(Action _action);
    bool active = false;

    // Script Functions
    void cs_print(const string& _value);
    void cs_clear();
    void cs_history();

private:
    uint curPos = 0;
    vector<string> history;
    forward_list<string> output;
    string input;
};

}
