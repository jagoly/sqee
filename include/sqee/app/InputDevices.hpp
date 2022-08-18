#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/core/Utilities.hpp>

//====== Forward Declarations ================================================//

extern "C" { typedef struct GLFWwindow GLFWwindow; }

namespace sq {

enum class Keyboard_Key : int8_t;
enum class Mouse_Button : int8_t;

class Window;
struct GamepadState;

//============================================================================//

/// Access to Keyboard / Mouse / Gamepad state.
class SQEE_API InputDevices : NonCopyable
{
public: //====================================================//

    /// Constructor.
    InputDevices(Window& window);

    //--------------------------------------------------------//

    /// Check if the given key is pressed.
    bool is_pressed(Keyboard_Key key) const;

    //--------------------------------------------------------//

    /// Get the location of the cursor.
    Vec2I get_cursor_location(bool flipY) const;

    /// Check if the given button is pressed.
    bool is_pressed(Mouse_Button button) const;

    /// Move the cursor to the window centre.
    Vec2I cursor_to_centre();

    //--------------------------------------------------------//

    /// Check if a gamepad is connected.
    bool check_gamepad_connected(int port) const;

    /// Get the current state of a gamepad.
    GamepadState poll_gamepad_state(int port) const;

private: //===================================================//

    GLFWwindow* const mGlfwWindow;
};

//============================================================================//

} // namespace sq
