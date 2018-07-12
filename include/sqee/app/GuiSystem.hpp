#pragma once

#include <functional>
#include <list>

#include <sqee/builtins.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Window; class InputDevices; struct GuiWidget; struct Event;

//============================================================================//

class GuiSystem final : NonCopyable
{
public: //====================================================//

    static GuiSystem& get() { return *impl_get_ptr_ref(); }

    static void construct(Window& window, InputDevices& inputDevices);

    static void destruct();

    //--------------------------------------------------------//

    bool handle_event(Event event);

    void finish_handle_events();

    void finish_scene_update(double elapsed);

    void render_gui();

    //--------------------------------------------------------//

    void show_imgui_demo();

    //--------------------------------------------------------//

    void enable_widget(GuiWidget& widget);
    void disable_widget(GuiWidget& widget);

    void draw_widgets();

private: //===================================================//

    GuiSystem() = default;
    ~GuiSystem() = default;

    static GuiSystem*& impl_get_ptr_ref();

    // list so we can add or remove during iteration
    std::list<GuiWidget*> mWidgets;
    std::list<GuiWidget*>::iterator mWidgetIter;

    class Implementation;
    friend class Implementation;
    unique_ptr<Implementation> impl;
};

//============================================================================//

struct GuiWidget final : NonCopyable
{
    GuiWidget() { GuiSystem::get().enable_widget(*this); }

    ~GuiWidget() { GuiSystem::get().disable_widget(*this); }

    std::function<void()> func = nullptr;
};

//============================================================================//

} // namespace sq
