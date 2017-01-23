#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

#include <sqee/app/Scene.hpp>

//============================================================================//

// Non-SQEE Forward Declarations /////

namespace sf { class Window; class Event; }
namespace chaiscript { class ChaiScript; }

//============================================================================//

namespace sq {

//============================================================================//

// Aliases and Forward Declarations /////

using ChaiEngine = chai::ChaiScript;

class ChaiConsole;
class DebugOverlay;
class MessageBus;

//============================================================================//

/// The SQEE Application class
class Application : NonCopyable
{
public:

    //========================================================//

    /// Constructor
    Application();

    /// Virtual Destructor
    virtual ~Application();

    //========================================================//

    int run();

    void quit(int code);

    //========================================================//

    virtual void update_options();
    virtual bool handle(sf::Event event);

    //========================================================//

    ChaiConsole& get_chai_console() { return *mChaiConsole; }
    ChaiEngine& get_chai_engine() { return *mChaiEngine; }
    DebugOverlay& get_debug_overlay() { return *mDebugOverlay; }
    MessageBus& get_message_bus() { return *mMessageBus; }

    //========================================================//

    string OPTION_WindowTitle  = "SQEE Application";
    Vec2U  OPTION_WindowSize   = { 800u, 600u };
    bool   OPTION_VerticalSync = false;
    bool   OPTION_HideCursor   = false;
    bool   OPTION_KeyRepeat    = false;

    Vec2F mouse_centre();

protected:

    //========================================================//

    unique_ptr<ChaiConsole> mChaiConsole;
    unique_ptr<ChaiEngine> mChaiEngine;
    unique_ptr<DebugOverlay> mDebugOverlay;
    unique_ptr<MessageBus> mMessageBus;

    //========================================================//

    vector<unique_ptr<Scene>> mScenes;
    unique_ptr<sf::Window> mWindow;

    int mReturnCode = -1;
};

//============================================================================//

} // namespace sq
