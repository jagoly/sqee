// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <functional>

#include <sqee/misc/Builtins.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Window; class InputDevices; struct Event;

//============================================================================//

class SQEE_API GuiSystem final : private NonCopyable
{
public: //====================================================//

    static GuiSystem& get();

    static void construct(Window& window, InputDevices& inputDevices);

    static void destruct();

    //--------------------------------------------------------//

    bool handle_event(Event event);

    void finish_handle_events(bool focus);

    void finish_scene_update(double elapsed);

    void render_gui();

    //--------------------------------------------------------//

    void show_imgui_demo();

private: //===================================================//

    GuiSystem() = default;
    ~GuiSystem() = default;

    class Implementation;
    friend class Implementation;
    UniquePtr<Implementation> impl;
};

//============================================================================//

} // namespace sq
