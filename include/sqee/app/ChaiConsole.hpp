#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/OrderedMap.hpp>

namespace sf { class Event; }

namespace sq {

/// The SQEE Chaiscript Console
class ChaiConsole final : NonCopyable {
public:
    ChaiConsole(Application* _app);

    void update();
    void render();

    void handle_input(sf::Event _event);

    void toggle_active();
    OrderedMap<string, function<void()>> onShowFuncs;
    OrderedMap<string, function<void()>> onHideFuncs;

    void cs_print(const string& _value);
    void cs_history(); void cs_clear();

    double accum = 0.0;
    bool active = false;

private:
    Application* const app;

    vector<string> history;
    forward_list<string> output;
    bool cursorVis = false;
    bool cvisDelay = false;
    uint cursorPos = 0u;
    int histInd = -1;
    string input;
};

}
