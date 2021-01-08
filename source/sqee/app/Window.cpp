#include <sqee/app/Window.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/core/Macros.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/debug/OpenGL.hpp>

#include <sqee/gl/Constants.hpp>
#include <sqee/gl/Functions.hpp>
#include <sqee/gl/Loader.hpp>

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowStyle.hpp>

using namespace sq;

//============================================================================//

static constexpr Event::Type conv_sfml_event_type[]
{
    Event::Type::Window_Close,      // Closed
    Event::Type::Window_Resize,     // Resized
    Event::Type::Window_Unfocus,    // LostFocus
    Event::Type::Window_Focus,      // GainedFocus
    Event::Type::Text_Entry,        // TextEntered
    Event::Type::Keyboard_Press,    // KeyPressed
    Event::Type::Keyboard_Release,  // KeyReleased
    Event::Type::Unknown,           // MouseWheelMoved
    Event::Type::Mouse_Scroll,      // MouseWheelScrolled
    Event::Type::Mouse_Press,       // Mouse_ButtonPressed
    Event::Type::Mouse_Release,     // Mouse_ButtonReleased
    Event::Type::Unknown,           // MouseMoved
    Event::Type::Unknown,           // MouseEntered
    Event::Type::Unknown,           // MouseLeft
    Event::Type::Gamepad_Press,     // JoystickButtonPressed
    Event::Type::Gamepad_Release,   // JoystickButtonReleased
    Event::Type::Unknown,           // JoystickMoved
    Event::Type::Unknown,           // JoystickConnected
    Event::Type::Unknown,           // JoystickDisconnected
    Event::Type::Unknown,           // TouchBegan
    Event::Type::Unknown,           // TouchMoved
    Event::Type::Unknown,           // TouchEnded
    Event::Type::Unknown,           // SensorChanged
};

//============================================================================//

static constexpr Keyboard_Key conv_sfml_keyboard_key[]
{
    Keyboard_Key::A,            // A
    Keyboard_Key::B,            // B
    Keyboard_Key::C,            // C
    Keyboard_Key::D,            // D
    Keyboard_Key::E,            // E
    Keyboard_Key::F,            // F
    Keyboard_Key::G,            // G
    Keyboard_Key::H,            // H
    Keyboard_Key::I,            // I
    Keyboard_Key::J,            // J
    Keyboard_Key::K,            // K
    Keyboard_Key::L,            // L
    Keyboard_Key::M,            // M
    Keyboard_Key::N,            // N
    Keyboard_Key::O,            // O
    Keyboard_Key::P,            // P
    Keyboard_Key::Q,            // Q
    Keyboard_Key::R,            // R
    Keyboard_Key::S,            // S
    Keyboard_Key::T,            // T
    Keyboard_Key::U,            // U
    Keyboard_Key::V,            // V
    Keyboard_Key::W,            // W
    Keyboard_Key::X,            // X
    Keyboard_Key::Y,            // Y
    Keyboard_Key::Z,            // Z
    Keyboard_Key::Num_0,        // Num0
    Keyboard_Key::Num_1,        // Num1
    Keyboard_Key::Num_2,        // Num2
    Keyboard_Key::Num_3,        // Num3
    Keyboard_Key::Num_4,        // Num4
    Keyboard_Key::Num_5,        // Num5
    Keyboard_Key::Num_6,        // Num6
    Keyboard_Key::Num_7,        // Num7
    Keyboard_Key::Num_8,        // Num8
    Keyboard_Key::Num_9,        // Num9
    Keyboard_Key::Escape,       // Escape
    Keyboard_Key::Control_L,    // LControl
    Keyboard_Key::Shift_L,      // LShift
    Keyboard_Key::Alt_L,        // LAlt
    Keyboard_Key::Super_L,      // LSystem
    Keyboard_Key::Control_R,    // RControl
    Keyboard_Key::Shift_R,      // RShift
    Keyboard_Key::Alt_R,        // RAlt
    Keyboard_Key::Super_R,      // RSystem
    Keyboard_Key::Menu,         // Menu
    Keyboard_Key::LeftBracket,  // LBracket
    Keyboard_Key::RightBracket, // RBracket
    Keyboard_Key::SemiColon,    // SemiColon
    Keyboard_Key::Comma,        // Comma
    Keyboard_Key::Period,       // Period
    Keyboard_Key::Apostrophe,   // Quote
    Keyboard_Key::Slash,        // Slash
    Keyboard_Key::BackSlash,    // BackSlash
    Keyboard_Key::Grave,        // Tilde
    Keyboard_Key::Equal,        // Equal
    Keyboard_Key::Dash,         // Dash
    Keyboard_Key::Space,        // Space
    Keyboard_Key::Return,       // Return
    Keyboard_Key::BackSpace,    // BackSpace
    Keyboard_Key::Tab,          // Tab
    Keyboard_Key::PageUp,       // PageUp
    Keyboard_Key::PageDown,     // PageDown
    Keyboard_Key::End,          // End
    Keyboard_Key::Home,         // Home
    Keyboard_Key::Insert,       // Insert
    Keyboard_Key::Delete,       // Delete
    Keyboard_Key::Pad_Plus,     // Add
    Keyboard_Key::Pad_Minus,    // Subtract
    Keyboard_Key::Pad_Multiply, // Multiply
    Keyboard_Key::Pad_Divide,   // Divide
    Keyboard_Key::Arrow_Left,   // Left
    Keyboard_Key::Arrow_Right,  // Right
    Keyboard_Key::Arrow_Up,     // Up
    Keyboard_Key::Arrow_Down,   // Down
    Keyboard_Key::Pad_0,        // Numpad0
    Keyboard_Key::Pad_1,        // Numpad1
    Keyboard_Key::Pad_2,        // Numpad2
    Keyboard_Key::Pad_3,        // Numpad3
    Keyboard_Key::Pad_4,        // Numpad4
    Keyboard_Key::Pad_5,        // Numpad5
    Keyboard_Key::Pad_6,        // Numpad6
    Keyboard_Key::Pad_7,        // Numpad7
    Keyboard_Key::Pad_8,        // Numpad8
    Keyboard_Key::Pad_9,        // Numpad9
    Keyboard_Key::F1,           // F1
    Keyboard_Key::F2,           // F2
    Keyboard_Key::F3,           // F3
    Keyboard_Key::F4,           // F4
    Keyboard_Key::F5,           // F5
    Keyboard_Key::F6,           // F6
    Keyboard_Key::F7,           // F7
    Keyboard_Key::F8,           // F8
    Keyboard_Key::F9,           // F9
    Keyboard_Key::F10,          // F10
    Keyboard_Key::F11,          // F11
    Keyboard_Key::F12,          // F12
    Keyboard_Key::Unknown,      // F13
    Keyboard_Key::Unknown,      // F14
    Keyboard_Key::Unknown,      // F15
    Keyboard_Key::Pause,        // Pause
};

//============================================================================//

static constexpr Mouse_Button conv_sfml_mouse_button[]
{
    Mouse_Button::Left,   // Left
    Mouse_Button::Right,  // Right
    Mouse_Button::Middle, // Middle
    Mouse_Button::ExtraA, // XButton1
    Mouse_Button::ExtraB, // XButton2
};

//============================================================================//

static constexpr Mouse_Wheel conv_sfml_mouse_wheel[]
{
    Mouse_Wheel::Vertical,   // Vertical
    Mouse_Wheel::Horizontal, // Horizontal
};

//============================================================================//

static constexpr Gamepad_Button conv_sfml_gamepad_button[]
{
    Gamepad_Button::A,       //  0 - A
    Gamepad_Button::B,       //  1 - B
    Gamepad_Button::X,       //  2 - X
    Gamepad_Button::Y,       //  3 - Y
    Gamepad_Button::L1,      //  4 - L1
    Gamepad_Button::R1,      //  5 - R1
    Gamepad_Button::Select,  //  6 - Select
    Gamepad_Button::Start,   //  7 - Start
    Gamepad_Button::Home,    //  8 - Home
    Gamepad_Button::Unknown, //  9 - L3
    Gamepad_Button::Unknown, // 10 - R3
    Gamepad_Button::Unknown, // 11
    Gamepad_Button::Unknown, // 12
    Gamepad_Button::Unknown, // 13
    Gamepad_Button::Unknown, // 14
    Gamepad_Button::Unknown, // 15
};

//============================================================================//

struct Window::Implementation
{
    String windowTitle = "";
    bool vsyncEnabled = false;
    bool cursorHidden = false;
    bool keyRepeat = true;
    sf::Window sfmlWindow;
    std::vector<Event> events;
};

//============================================================================//

Window::Window(String title, Vec2U size)

    : impl(std::make_unique<Implementation>())
    , mSystemWindowPtr(&impl->sfmlWindow)
{
    //--------------------------------------------------------//

    sf::VideoMode mode { size.x, size.y, 32u };
    sf::Uint32 style = sf::Style::Default;
    sf::ContextSettings settings { 24u, 8u, 0u, 4u, 5u, sf::ContextSettings::Core | sf::ContextSettings::Debug };

    impl->sfmlWindow.create(mode, title, style, settings);

    impl->windowTitle = std::move(title);

    //--------------------------------------------------------//

    if (load_opengl_core45_functions() == false)
        log_warning("Failed to load some OpenGL functions!");

    //--------------------------------------------------------//

    #ifdef SQEE_DEBUG

    const GLubyte* renderer = gl::GetString(gl::RENDERER);
    const GLubyte* version = gl::GetString(gl::VERSION);
    log_info("Renderer: {}", reinterpret_cast<const char*>(renderer));
    log_info("Version: {}", reinterpret_cast<const char*>(version));

    gl::Enable(gl::DEBUG_OUTPUT);
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);

    // generally, we don't care about notification messages so just ignore them
    gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);

    // we use glGetShaderInfoLog, but some drivers also send logs to the debug callback
    //gl::DebugMessageControl(gl::DEBUG_SOURCE_SHADER_COMPILER, gl::DEBUG_TYPE_ERROR, gl::DEBUG_SEVERITY_HIGH, 0, nullptr, false);

    gl::DebugMessageCallback(debug_callback, nullptr);

    #endif
}

//============================================================================//

Window::~Window() noexcept = default;

//============================================================================//

void Window::set_window_title(String title)
{
    impl->windowTitle = title;
    impl->sfmlWindow.setTitle(title);
}

void Window::set_window_size(Vec2U size)
{
    if (size == get_window_size()) return;
    impl->sfmlWindow.setSize({size.x, size.y});
}

void Window::set_vsync_enabled(bool enabled)
{
    impl->vsyncEnabled = enabled;
    impl->sfmlWindow.setVerticalSyncEnabled(enabled);
}

void Window::set_cursor_hidden(bool hidden)
{
    impl->cursorHidden = hidden;
    impl->sfmlWindow.setMouseCursorVisible(!hidden);
}

void Window::set_key_repeat(bool repeat)
{
    impl->keyRepeat = repeat;
    impl->sfmlWindow.setKeyRepeatEnabled(repeat);
}

//============================================================================//

const String& Window::get_window_title() const
{
    return impl->windowTitle;
}

Vec2U Window::get_window_size() const
{
    auto size = impl->sfmlWindow.getSize();
    return { size.x, size.y };
}

bool Window::get_vsync_enabled() const
{
    return impl->vsyncEnabled;
}

bool Window::get_cursor_hidden() const
{
    return impl->cursorHidden;
}

bool Window::get_key_repeat() const
{
    return impl->keyRepeat;
}

//============================================================================//

bool Window::has_focus() const
{
    return impl->sfmlWindow.hasFocus();
}

//============================================================================//

const std::vector<Event>& Window::fetch_events()
{
    impl->events.clear();

    sf::Event sfe;

    while (impl->sfmlWindow.pollEvent(sfe))
    {
        Event event { conv_sfml_event_type[sfe.type], {} };

        SWITCH ( event.type ) {

        CASE ( Unknown ) continue; // don't do push_back()

        CASE ( Window_Close, Window_Focus, Window_Unfocus ); // no extra data

        CASE ( Window_Resize )
        {
            event.data.resize = { { sfe.size.width, sfe.size.height } };
        }

        CASE ( Keyboard_Press, Keyboard_Release )
        {
            event.data.keyboard =
            {
                conv_sfml_keyboard_key[sfe.key.code],
                sfe.key.shift, sfe.key.control, sfe.key.alt, sfe.key.system
            };
        }

        CASE ( Mouse_Press, Mouse_Release )
        {
            event.data.mouse =
            {
                conv_sfml_mouse_button[sfe.mouseButton.button],
                //{ sfe.mouseButton.x, int(get_window_size().y) - sfe.mouseButton.y }
                { sfe.mouseButton.x, sfe.mouseButton.y }
            };
        }

        CASE ( Gamepad_Press, Gamepad_Release )
        {
            event.data.gamepad =
            {
                int32_t(sfe.joystickConnect.joystickId),
                conv_sfml_gamepad_button[sfe.joystickButton.button]
            };
        }

        CASE ( Mouse_Scroll )
        {
            event.data.scroll =
            {
                conv_sfml_mouse_wheel[sfe.mouseWheelScroll.wheel],
                sfe.mouseWheelScroll.delta
            };
        }

        CASE ( Text_Entry )
        {
            event.data.text = { sfe.text.unicode };
        }

        } SWITCH_END;

        impl->events.push_back(event);
    }

    return impl->events;
}

//============================================================================//

void Window::swap_buffers()
{
    impl->sfmlWindow.display();
}
