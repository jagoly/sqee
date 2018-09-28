// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/InputDevices.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Window.hpp>

#include <sqee/app/Window.hpp>
#include <sqee/debug/Assert.hpp>

using namespace sq;

//============================================================================//

InputDevices::InputDevices(Window& window)
    : mSystemWindowPtr(window.mSystemWindowPtr)
{}

//============================================================================//

bool InputDevices::is_pressed(Keyboard_Key key) const
{
    SQASSERT(key != Keyboard_Key::Unknown, "");

    constexpr sf::Keyboard::Key sqee_to_sfml[]
    {
        sf::Keyboard::Key::Num0,  // Num_0
        sf::Keyboard::Key::Num1,  // Num_1
        sf::Keyboard::Key::Num2,  // Num_2
        sf::Keyboard::Key::Num3,  // Num_3
        sf::Keyboard::Key::Num4,  // Num_4
        sf::Keyboard::Key::Num5,  // Num_5
        sf::Keyboard::Key::Num6,  // Num_6
        sf::Keyboard::Key::Num7,  // Num_7
        sf::Keyboard::Key::Num8,  // Num_8
        sf::Keyboard::Key::Num9,  // Num_9

        sf::Keyboard::Key::A,  // A
        sf::Keyboard::Key::B,  // B
        sf::Keyboard::Key::C,  // C
        sf::Keyboard::Key::D,  // D
        sf::Keyboard::Key::E,  // E
        sf::Keyboard::Key::F,  // F
        sf::Keyboard::Key::G,  // G
        sf::Keyboard::Key::H,  // H
        sf::Keyboard::Key::I,  // I
        sf::Keyboard::Key::J,  // J
        sf::Keyboard::Key::K,  // K
        sf::Keyboard::Key::L,  // L
        sf::Keyboard::Key::M,  // M
        sf::Keyboard::Key::N,  // N
        sf::Keyboard::Key::O,  // O
        sf::Keyboard::Key::P,  // P
        sf::Keyboard::Key::Q,  // Q
        sf::Keyboard::Key::R,  // R
        sf::Keyboard::Key::S,  // S
        sf::Keyboard::Key::T,  // T
        sf::Keyboard::Key::U,  // U
        sf::Keyboard::Key::V,  // V
        sf::Keyboard::Key::W,  // W
        sf::Keyboard::Key::X,  // X
        sf::Keyboard::Key::Y,  // Y
        sf::Keyboard::Key::Z,  // Z

        sf::Keyboard::Key::Tilde,      // Grave
        sf::Keyboard::Key::Dash,       // Dash
        sf::Keyboard::Key::Equal,      // Equal
        sf::Keyboard::Key::LBracket,   // LeftBracket
        sf::Keyboard::Key::RBracket,   // RightBracket
        sf::Keyboard::Key::BackSlash,  // BackSlash

        sf::Keyboard::Key::SemiColon,  // SemiColon
        sf::Keyboard::Key::Quote,      // Apostrophe
        sf::Keyboard::Key::Comma,      // Comma
        sf::Keyboard::Key::Period,     // Period
        sf::Keyboard::Key::Slash,      // Slash

        sf::Keyboard::Key::LShift,    // Shift_L
        sf::Keyboard::Key::RShift,    // Shift_R
        sf::Keyboard::Key::LControl,  // Control_L
        sf::Keyboard::Key::RControl,  // Control_R
        sf::Keyboard::Key::LAlt,      // Alt_L
        sf::Keyboard::Key::RAlt,      // Alt_R
        sf::Keyboard::Key::LSystem,   // Super_L
        sf::Keyboard::Key::RSystem,   // Super_R

        sf::Keyboard::Key::Menu,       // Menu
        sf::Keyboard::Key::BackSpace,  // BackSpace
        sf::Keyboard::Key::Tab,        // Tab
        sf::Keyboard::Key::Return,     // Return
        sf::Keyboard::Key::Space,      // Space
        sf::Keyboard::Key::Escape,     // Escape
        sf::Keyboard::Key::Pause,      // Pause

        sf::Keyboard::Key::Insert,    // Insert
        sf::Keyboard::Key::Delete,    // Delete
        sf::Keyboard::Key::Home,      // Home
        sf::Keyboard::Key::End,       // End
        sf::Keyboard::Key::PageUp,    // PageUp
        sf::Keyboard::Key::PageDown,  // PageDown

        sf::Keyboard::Key::Left,   // Arrow_Left
        sf::Keyboard::Key::Up,     // Arrow_Up
        sf::Keyboard::Key::Right,  // Arrow_Right
        sf::Keyboard::Key::Down,   // Arrow_Down

        sf::Keyboard::Key::Numpad0,  // Pad_0
        sf::Keyboard::Key::Numpad1,  // Pad_1
        sf::Keyboard::Key::Numpad2,  // Pad_2
        sf::Keyboard::Key::Numpad3,  // Pad_3
        sf::Keyboard::Key::Numpad4,  // Pad_4
        sf::Keyboard::Key::Numpad5,  // Pad_5
        sf::Keyboard::Key::Numpad6,  // Pad_6
        sf::Keyboard::Key::Numpad7,  // Pad_7
        sf::Keyboard::Key::Numpad8,  // Pad_8
        sf::Keyboard::Key::Numpad9,  // Pad_9

        sf::Keyboard::Key::Add,       // Pad_Plus
        sf::Keyboard::Key::Subtract,  // Pad_Minus
        sf::Keyboard::Key::Multiply,  // Pad_Multiply
        sf::Keyboard::Key::Divide,    // Pad_Divide

        sf::Keyboard::Key::F1,   // F1
        sf::Keyboard::Key::F2,   // F2
        sf::Keyboard::Key::F3,   // F3
        sf::Keyboard::Key::F4,   // F4
        sf::Keyboard::Key::F5,   // F5
        sf::Keyboard::Key::F6,   // F6
        sf::Keyboard::Key::F7,   // F7
        sf::Keyboard::Key::F8,   // F8
        sf::Keyboard::Key::F9,   // F9
        sf::Keyboard::Key::F10,  // F10
        sf::Keyboard::Key::F11,  // F11
        sf::Keyboard::Key::F12,  // F12
    };

    return sf::Keyboard::isKeyPressed(sqee_to_sfml[int(key)]);
}

//============================================================================//

Vec2U InputDevices::get_cursor_location() const
{
    auto window = static_cast<const sf::Window*>(mSystemWindowPtr);

    sf::Vector2i size ( window->getSize() );
    sf::Vector2i position = sf::Mouse::getPosition(*window);

    //position.y = size.y - position.y;

    uint clampedX = uint(maths::clamp(position.x, 0, size.x));
    uint clampedY = uint(maths::clamp(position.y, 0, size.y));

    return { clampedX, clampedY };
}

bool InputDevices::is_pressed(Mouse_Button button) const
{
    SQASSERT(button != Mouse_Button::Unknown, "");

    constexpr sf::Mouse::Button sqee_to_sfml[]
    {
        sf::Mouse::Button::Left,      // Left
        sf::Mouse::Button::Right,     // Right
        sf::Mouse::Button::Middle,    // Middle
        sf::Mouse::Button::XButton1,  // ExtraA
        sf::Mouse::Button::XButton2,  // ExtraB
    };

    return sf::Mouse::isButtonPressed(sqee_to_sfml[int(button)]);
}

Vec2I InputDevices::cursor_to_centre()
{
    auto window = static_cast<sf::Window*>(mSystemWindowPtr);

    sf::Vector2i centre ( window->getSize() / 2u );
    sf::Vector2i position = sf::Mouse::getPosition(*window);
    sf::Mouse::setPosition(centre, *window);

    //return { centre.x - position.x, -(centre.y - position.y) };
    return { centre.x - position.x, centre.y - position.y };
}

//============================================================================//

Vec2F InputDevices::get_stick_pos(int32_t port, Gamepad_Stick stick) const
{
    SQASSERT(port >= 0 && stick != Gamepad_Stick::Unknown, "");

    if (stick == Gamepad_Stick::Left)
    {
        const float x = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::X);
        const float y = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::Y);
        return Vec2F ( x, -y ) / 100.f;
    }

    if (stick == Gamepad_Stick::Right)
    {
        const float x = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::U);
        const float y = sf::Joystick::getAxisPosition(uint(port), sf::Joystick::V);
        return Vec2F ( x, -y ) / 100.f;
    }

    return Vec2F(0.f, 0.f);
}

bool InputDevices::is_pressed(int32_t port, Gamepad_Button button) const
{
    SQASSERT(port >= 0 && button != Gamepad_Button::Unknown, "");

    constexpr uint sqee_to_sfml[]
    {
        0u, // A
        1u, // B
        2u, // X
        3u, // Y
        4u, // L1
        5u, // R1
        6u, // Select
        7u, // Start
        8u, // Home
    };

    return sf::Joystick::isButtonPressed(uint(port), sqee_to_sfml[int(button)]);
}
