#pragma once

#include <sqee/builtins.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Window; class InputDevices; struct Event;

//============================================================================//

class GuiSystem final : NonCopyable
{
public: //====================================================//

    GuiSystem(Window& window, InputDevices& inputDevices);

    ~GuiSystem();

    //--------------------------------------------------------//

    bool handle_event(Event event);

    void begin_new_frame(double elapsed);

    void render_gui();

private: //===================================================//

    class Implementation;
    friend class Implementation;
    unique_ptr<Implementation> impl;
};

//============================================================================//

} // namespace sq
