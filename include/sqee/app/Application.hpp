#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

// Forward Declarations /////
namespace sf { class Window; class Event; }
namespace chaiscript { class ChaiScript; }

namespace sq {

class DebugOverlay; class ChaiConsole; class Scene;
using ChaiEngine = chai::ChaiScript;

/// The basic SQEE Application class
class Application : NonCopyable {
public:
    Application();
    virtual ~Application();

    int run();
    void quit(int _code);

    virtual void update_options();
    virtual bool handle(sf::Event _event);

    unique_ptr<ChaiEngine> chaiEngine;
    unique_ptr<DebugOverlay> overlay;
    unique_ptr<ChaiConsole> console;

    string OPTION_WindowTitle  = "SQEE Application";
    Vec2U  OPTION_WindowSize   = {800u, 600u};
    bool   OPTION_VerticalSync = false;
    bool   OPTION_HideCursor   = false;
    bool   OPTION_KeyRepeat    = false;

    Vec2F mouse_centre();

protected:
    vector<unique_ptr<Scene>> activeScenes;
    unique_ptr<sf::Window> window;
    int returnCode = -1;
};

}
