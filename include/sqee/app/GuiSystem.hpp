// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class InputDevices;
class Window;
struct Event;

//============================================================================//

class SQEE_API GuiSystem final : private NonCopyable
{
public: //====================================================//

    static GuiSystem& get();

    static void construct(Window& window, InputDevices& inputDevices);

    static void destruct();

    //--------------------------------------------------------//

    void set_style_widgets_default();
    void set_style_widgets_supertux();

    void set_style_colours_classic();
    void set_style_colours_dark();
    void set_style_colours_light();
    void set_style_colours_supertux();

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
    std::unique_ptr<Implementation> impl;

    friend Implementation;
};

//============================================================================//

} // namespace sq
