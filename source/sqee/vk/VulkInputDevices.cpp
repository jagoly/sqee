#include <sqee/vk/VulkInputDevices.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/vk/VulkWindow.hpp>
#include <sqee/debug/Assert.hpp>

#include <GLFW/glfw3.h>

using namespace sq;

//============================================================================//

VulkInputDevices::VulkInputDevices(VulkWindow& window)
    : mGlfwWindow(window.get_glfw_window()) {}

//============================================================================//

bool VulkInputDevices::is_pressed(Keyboard_Key key) const
{
    SQASSERT(key != Keyboard_Key::Unknown, "");

    constexpr int sqee_to_glfw[]
    {
        GLFW_KEY_0,  // Num_0
        GLFW_KEY_1,  // Num_1
        GLFW_KEY_2,  // Num_2
        GLFW_KEY_3,  // Num_3
        GLFW_KEY_4,  // Num_4
        GLFW_KEY_5,  // Num_5
        GLFW_KEY_6,  // Num_6
        GLFW_KEY_7,  // Num_7
        GLFW_KEY_8,  // Num_8
        GLFW_KEY_9,  // Num_9

        GLFW_KEY_A,  // A
        GLFW_KEY_B,  // B
        GLFW_KEY_C,  // C
        GLFW_KEY_D,  // D
        GLFW_KEY_E,  // E
        GLFW_KEY_F,  // F
        GLFW_KEY_G,  // G
        GLFW_KEY_H,  // H
        GLFW_KEY_I,  // I
        GLFW_KEY_J,  // J
        GLFW_KEY_K,  // K
        GLFW_KEY_L,  // L
        GLFW_KEY_M,  // M
        GLFW_KEY_N,  // N
        GLFW_KEY_O,  // O
        GLFW_KEY_P,  // P
        GLFW_KEY_Q,  // Q
        GLFW_KEY_R,  // R
        GLFW_KEY_S,  // S
        GLFW_KEY_T,  // T
        GLFW_KEY_U,  // U
        GLFW_KEY_V,  // V
        GLFW_KEY_W,  // W
        GLFW_KEY_X,  // X
        GLFW_KEY_Y,  // Y
        GLFW_KEY_Z,  // Z

        GLFW_KEY_GRAVE_ACCENT,   // Grave
        GLFW_KEY_MINUS,          // Dash
        GLFW_KEY_EQUAL,          // Equal
        GLFW_KEY_LEFT_BRACKET,   // LeftBracket
        GLFW_KEY_RIGHT_BRACKET,  // RightBracket
        GLFW_KEY_BACKSLASH,      // BackSlash

        GLFW_KEY_SEMICOLON,   // SemiColon
        GLFW_KEY_APOSTROPHE,  // Apostrophe
        GLFW_KEY_COMMA,       // Comma
        GLFW_KEY_PERIOD,      // Period
        GLFW_KEY_SLASH,       // Slash

        GLFW_KEY_LEFT_SHIFT,     // Shift_L
        GLFW_KEY_RIGHT_SHIFT,    // Shift_R
        GLFW_KEY_LEFT_CONTROL,   // Control_L
        GLFW_KEY_RIGHT_CONTROL,  // Control_R
        GLFW_KEY_LEFT_ALT,       // Alt_L
        GLFW_KEY_RIGHT_ALT,      // Alt_R
        GLFW_KEY_LEFT_SUPER,     // Super_L
        GLFW_KEY_RIGHT_SUPER,    // Super_R

        GLFW_KEY_MENU,       // Menu
        GLFW_KEY_BACKSPACE,  // BackSpace
        GLFW_KEY_TAB,        // Tab
        GLFW_KEY_ENTER,      // Return
        GLFW_KEY_SPACE,      // Space
        GLFW_KEY_ESCAPE,     // Escape
        GLFW_KEY_PAUSE,      // Pause

        GLFW_KEY_INSERT,     // Insert
        GLFW_KEY_DELETE,     // Delete
        GLFW_KEY_HOME,       // Home
        GLFW_KEY_END,        // End
        GLFW_KEY_PAGE_UP,    // PageUp
        GLFW_KEY_PAGE_DOWN,  // PageDown

        GLFW_KEY_LEFT,   // Arrow_Left
        GLFW_KEY_UP,     // Arrow_Up
        GLFW_KEY_RIGHT,  // Arrow_Right
        GLFW_KEY_DOWN,   // Arrow_Down

        GLFW_KEY_KP_0,  // Pad_0
        GLFW_KEY_KP_1,  // Pad_1
        GLFW_KEY_KP_2,  // Pad_2
        GLFW_KEY_KP_3,  // Pad_3
        GLFW_KEY_KP_4,  // Pad_4
        GLFW_KEY_KP_5,  // Pad_5
        GLFW_KEY_KP_6,  // Pad_6
        GLFW_KEY_KP_7,  // Pad_7
        GLFW_KEY_KP_8,  // Pad_8
        GLFW_KEY_KP_9,  // Pad_9

        GLFW_KEY_KP_DECIMAL,   // Pad_Decimal
        GLFW_KEY_KP_ADD,       // Pad_Plus
        GLFW_KEY_KP_SUBTRACT,  // Pad_Minus
        GLFW_KEY_KP_MULTIPLY,  // Pad_Multiply
        GLFW_KEY_KP_DIVIDE,    // Pad_Divide

        GLFW_KEY_F1,   // F1
        GLFW_KEY_F2,   // F2
        GLFW_KEY_F3,   // F3
        GLFW_KEY_F4,   // F4
        GLFW_KEY_F5,   // F5
        GLFW_KEY_F6,   // F6
        GLFW_KEY_F7,   // F7
        GLFW_KEY_F8,   // F8
        GLFW_KEY_F9,   // F9
        GLFW_KEY_F10,  // F10
        GLFW_KEY_F11,  // F11
        GLFW_KEY_F12,  // F12
    };

    return bool(glfwGetKey(mGlfwWindow, sqee_to_glfw[int(key)]));
}

//============================================================================//

Vec2I VulkInputDevices::get_cursor_location(bool flipY) const
{
    double dblX, dblY;
    glfwGetCursorPos(mGlfwWindow, &dblX, &dblY);

    int resultX = int(std::floor(dblX));
    int resultY = int(std::floor(dblY));

    if (flipY == true)
    {
        int windowWidth, windowHeight;
        glfwGetWindowSize(mGlfwWindow, &windowWidth, &windowHeight);
        resultY = windowHeight - resultY;
    }

    return { resultX, resultY };
}

bool VulkInputDevices::is_pressed(Mouse_Button button) const
{
    SQASSERT(button != Mouse_Button::Unknown, "");

    constexpr int sqee_to_glfw[]
    {
        GLFW_MOUSE_BUTTON_1,  // Left
        GLFW_MOUSE_BUTTON_2,  // Right
        GLFW_MOUSE_BUTTON_3,  // Middle
        GLFW_MOUSE_BUTTON_4,  // ExtraA
        GLFW_MOUSE_BUTTON_5,  // ExtraB
    };

    return bool(glfwGetMouseButton(mGlfwWindow, sqee_to_glfw[int(button)]));
}

Vec2I VulkInputDevices::cursor_to_centre()
{
    std::abort(); // todo
//    sf::Vector2i centre ( window->getSize() / 2u );
//    sf::Vector2i position = sf::Mouse::getPosition(*window);
//    sf::Mouse::setPosition(centre, *window);

//    //return { centre.x - position.x, -(centre.y - position.y) };
//    return { centre.x - position.x, centre.y - position.y };
}

//============================================================================//

//Vec2F InputDevices::get_stick_pos(int32_t port, Gamepad_Stick stick) const
//{
//    SQASSERT(port >= 0 && stick != Gamepad_Stick::Unknown, "");

//    if (stick == Gamepad_Stick::Left)
//    {
//        const float x = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::X);
//        const float y = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::Y);
//        return Vec2F ( x, -y ) / 100.f;
//    }

//    if (stick == Gamepad_Stick::Right)
//    {
//        const float x = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::U);
//        const float y = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::V);
//        return Vec2F ( x, -y ) / 100.f;
//    }

//    return Vec2F(0.f, 0.f);
//}

//bool InputDevices::is_pressed(int32_t port, Gamepad_Button button) const
//{
//    SQASSERT(port >= 0 && button != Gamepad_Button::Unknown, "");

//    constexpr uint sqee_to_sfml[]
//    {
//        0u, // A
//        1u, // B
//        2u, // X
//        3u, // Y
//        4u, // L1
//        5u, // R1
//        6u, // Select
//        7u, // Start
//        8u, // Home
//    };

//    return sf::Joystick::isButtonPressed(uint(port), sqee_to_sfml[int(button)]);
//}
