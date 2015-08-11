#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>

namespace sq {

class ConsoleScene : public Scene {
public:
    ConsoleScene(Application* _app);

    enum class Action { Up, Down, Left, Right,
                        Backspace, Delete, Home, End,
                        Return, ShiftReturn };

    void update();
    void render(float _ft);

    void exec();
    void handle_character(char _char);
    void handle_action(Action _action);
    bool active = false;

    void cs_print(const string& _value);
    void cs_history(); void cs_clear();

private:
    uint curPos = 0u;
    vector<string> history;
    forward_list<string> output;
    bool tickSwitch = false;
    bool tickDelay = false;
    int histInd = -1;
    string input;
};

}
