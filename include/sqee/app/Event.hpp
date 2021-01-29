// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

enum class Keyboard_Key : int8_t
{
    Unknown = -1,
    Num_0, Num_1, Num_2, Num_3, Num_4, Num_5, Num_6, Num_7, Num_8, Num_9,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Grave, Dash, Equal, LeftBracket, RightBracket, BackSlash,
    SemiColon, Apostrophe, Comma, Period, Slash,
    Shift_L, Shift_R, Control_L, Control_R, Alt_L, Alt_R, Super_L, Super_R,
    Menu, BackSpace, Tab, Return, Space, Escape, Pause,
    Insert, Delete, Home, End, PageUp, PageDown,
    Arrow_Left, Arrow_Up, Arrow_Right, Arrow_Down,
    Pad_0, Pad_1, Pad_2, Pad_3, Pad_4, Pad_5, Pad_6, Pad_7, Pad_8, Pad_9,
    Pad_Decimal, Pad_Plus, Pad_Minus, Pad_Multiply, Pad_Divide,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum class Mouse_Button : int8_t
{
    Unknown = -1,
    Left, Right, Middle,
    ExtraA, ExtraB
};

enum class Mouse_Wheel : int8_t
{
    Unknown = -1,
    Vertical, Horizontal
};

enum class Gamepad_Button : int8_t
{
    Unknown = -1,
    A, B, X, Y, L1, R1,
    Select, Start, Home
};

enum class Gamepad_Stick : int8_t
{
    Unknown = -1,
    Left, Right
};

//============================================================================//

//struct Event_WindowClose {};

//struct Event_WindowResize {};

//struct Event_WindowFocus {};

//struct Event_WindowUnfocus {};

//struct Event_KeyPress { Keyboard_Key key; bool shift, ctrl, alt, super; };

//struct Event_KeyRelease { Keyboard_Key key; bool shift, ctrl, alt, super; };

//struct Event_MousePress { Mouse_Button button; Vec2I position; };

//struct Event_MouseRelease { Mouse_Button button; Vec2I position; };

//struct Event_GamepadPress { int32_t port; Gamepad_Button button; };

//struct Event_GamepadRelease { int32_t port; Gamepad_Button button; };

//struct Event_MouseScroll { Mouse_Wheel wheel; float delta; };

//struct Event_TextEntry { uint32_t unicode; };

////============================================================================//

//using Event = std::variant <
//    Event_WindowClose, Event_WindowResize,
//    Event_WindowFocus, Event_WindowUnfocus,
//    Event_KeyPress, Event_KeyRelease,
//    Event_MousePress, Event_MouseRelease,
//    Event_GamepadPress, Event_GamepadRelease,
//    Event_MouseScroll, Event_TextEntry
//>;

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
        Mouse_Scroll,
        Text_Entry
    };

    union Data
    {
        struct { Vec2U size; }                                     resize;
        struct { Keyboard_Key key; bool shift, ctrl, alt, super; } keyboard;
        struct { Mouse_Button button; Vec2I position; }            mouse;
        struct { int32_t port; Gamepad_Button button; }            gamepad;
        struct { Mouse_Wheel wheel; float delta; }                 scroll;
        struct { uint32_t unicode; }                               text;
    };

    Type type = Type::Unknown;
    Data data {};
};

static_assert(sizeof(Event) == 16, "");

//============================================================================//

} // namespace sq
