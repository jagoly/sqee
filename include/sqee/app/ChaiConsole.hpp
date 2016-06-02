#pragma once

#include <forward_list>
#include <functional>

#include <sqee/builtins.hpp>
#include <sqee/misc/OrderedMap.hpp>

// Forward Declarations /////
namespace sq { class Application; }
namespace sf { class Event; }

namespace sq {

/// The SQEE Chaiscript Console
class ChaiConsole final : NonCopyable {
public:
    ChaiConsole(Application& _app);

    void tick(); void render();
    void handle_input(sf::Event _event);

    void toggle_active();
    OrderedMap<string, std::function<void()>> onShowFuncs;
    OrderedMap<string, std::function<void()>> onHideFuncs;

    void cs_print(const string& _value);
    void cs_history(); void cs_clear();

    double accumulation = 0.0;
    bool active = false;

private:
    Application& app;

    string input;
    vector<string> history;
    std::forward_list<string> output;
    bool cursorVis = false;
    bool cvisDelay = false;
    uint cursorPos = 0u;
    int histInd = -1;
};

}
