#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/app/InputDevices.hpp>

namespace sq {

//============================================================================//

struct Event final
{
    enum class Type
    {
        Unknown = -1,
        Window_Close,
        Window_Resize,
        Window_Focus,
        Window_Unfocus,
        Keyboard_Press,
        Keyboard_Release,
        Mouse_Press,
        Mouse_Release,
        Gamepad_Press,
        Gamepad_Release,
        Text_Entry
    };

    union Data
    {
        struct { Vec2U size; }                                     window_resize;
        struct { Keyboard_Key key; bool shift, ctrl, alt, super; } keyboard;
        struct { Mouse_Button button; Vec2I position; }            mouse;
        struct { int32_t port; Gamepad_Button button; }            gamepad;
        struct { uint32_t unicode; }                               text_entry;
    };

    Type type = Type::Unknown;
    Data data;
};

static_assert(sizeof(Event) == 16, "");

//============================================================================//

} // namespace sq
