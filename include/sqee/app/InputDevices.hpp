#pragma once

#include <sqee/maths/Vectors.hpp>

namespace sq {

//============================================================================//

class Window; // Forward Declaration

//============================================================================//

enum class Keyboard_Key
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
    Pad_Plus, Pad_Minus, Pad_Multiply, Pad_Divide,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
};

enum class Mouse_Button
{
    Unknown = -1,
    Left, Right, Middle,
    ExtraA, ExtraB
};

enum class Gamepad_Stick
{
    Unknown = -1,
    Left, Right
};

enum class Gamepad_Button
{
    Unknown = -1,
    DP_Left, DP_Right, DP_Up, DP_Down,
    A, B, X, Y, L1, R1, L2, R2,
    Select, Home, Start
};

//============================================================================//

/// Access to Keyboard / Mouse / Gamepad state.
class InputDevices final : NonCopyable
{
public: //====================================================//

    /// Constructor.
    InputDevices(Window& window);

    //--------------------------------------------------------//

    /// Check if the given key is pressed.
    bool is_pressed(Keyboard_Key key) const;

    //--------------------------------------------------------//

    /// Get the location of the cursor.
    Vec2U get_cursor_location() const;

    /// Check if the given button is pressed.
    bool is_pressed(Mouse_Button button) const;

    /// Move the cursor to the window centre.
    Vec2I cursor_to_centre();

    //--------------------------------------------------------//

    /// Get position of the given analogue stick.
    Vec2F get_stick_pos(int32_t port, Gamepad_Stick stick) const;

    /// Check if the given button is pressed.
    bool is_pressed(int32_t port, Gamepad_Button button) const;

private: //===================================================//

    void* const mSystemWindowPtr;
};

//============================================================================//

} // namespace sq
