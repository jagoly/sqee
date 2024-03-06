// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>

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
    Menu, BackSpace, Tab, Return, Space, Escape,
    Insert, Delete, Home, End, PageUp, PageDown,
    Arrow_Left, Arrow_Up, Arrow_Right, Arrow_Down,
    Pad_0, Pad_1, Pad_2, Pad_3, Pad_4, Pad_5, Pad_6, Pad_7, Pad_8, Pad_9,
    Pad_Decimal, Pad_Plus, Pad_Minus, Pad_Multiply, Pad_Divide, Pad_Return,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum class Mouse_Button : int8_t
{
    Unknown = -1,
    Left, Right, Middle,
    ExtraA, ExtraB
};

//============================================================================//

struct Event final
{
    enum class Type
    {
        Unknown = -1,
        Window_Close,
        Window_Resize,
        Window_Unfocus,
        Window_Focus,
        Keyboard_Release,
        Keyboard_Press,
        Keyboard_Repeat,
        Mouse_Release,
        Mouse_Press,
        Mouse_Scroll,
        Text_Entry
    };

    union Data
    {
        struct { Vec2U size; }                                        resize;
        struct { Keyboard_Key key; bool shift, ctrl, alt, super; }    keyboard;
        struct { Mouse_Button button; bool shift, ctrl, alt, super; } mouse;
        struct { Vec2F delta; }                                       scroll;
        struct { uint32_t unicode; }                                  text;
    };

    Type type = Type::Unknown;
    Data data {};
};

//============================================================================//

} // namespace sq

SQEE_ENUM_HELPER
(
    sq::Keyboard_Key,
    Unknown,
    Num_0, Num_1, Num_2, Num_3, Num_4, Num_5, Num_6, Num_7, Num_8, Num_9,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Grave, Dash, Equal, LeftBracket, RightBracket, BackSlash,
    SemiColon, Apostrophe, Comma, Period, Slash,
    Shift_L, Shift_R, Control_L, Control_R, Alt_L, Alt_R, Super_L, Super_R,
    Menu, BackSpace, Tab, Return, Space, Escape,
    Insert, Delete, Home, End, PageUp, PageDown,
    Arrow_Left, Arrow_Up, Arrow_Right, Arrow_Down,
    Pad_0, Pad_1, Pad_2, Pad_3, Pad_4, Pad_5, Pad_6, Pad_7, Pad_8, Pad_9,
    Pad_Decimal, Pad_Plus, Pad_Minus, Pad_Multiply, Pad_Divide, Pad_Return,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
)

SQEE_ENUM_HELPER
(
    sq::Mouse_Button,
    Unknown,
    Left, Right, Middle,
    ExtraA, ExtraB
)
