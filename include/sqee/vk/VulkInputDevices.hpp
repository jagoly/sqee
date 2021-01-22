#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

//====== Forward Declarations ================================================//

extern "C" { typedef struct GLFWwindow GLFWwindow; };

namespace sq {

class VulkWindow;

enum class Keyboard_Key : int8_t;
enum class Mouse_Button : int8_t;
enum class Gamepad_Button : int8_t;
enum class Gamepad_Stick : int8_t;

//============================================================================//

/// Access to Keyboard / Mouse / Gamepad state.
class SQEE_API VulkInputDevices final : private NonCopyable
{
public: //====================================================//

    /// Constructor.
    VulkInputDevices(VulkWindow& window);

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

    /// Get position of the given analogue stick.
    Vec2F get_stick_pos(int32_t port, Gamepad_Stick stick) const;

    /// Check if the given button is pressed.
    bool is_pressed(int32_t port, Gamepad_Button button) const;

private: //===================================================//

    GLFWwindow* const mGlfwWindow;
};

//============================================================================//

} // namespace sq
