// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>

#include <sqee/app/Event.hpp>

namespace sq {

//============================================================================//

/// The SQEE Window class.
class SQEE_API Window final : private NonCopyable
{
public: //====================================================//

    /// Constructor.
    Window(String title, Vec2U size);

    /// Destructor.
    ~Window();

    //--------------------------------------------------------//

    /// Set the title of the Window.
    void set_window_title(String title);

    /// Set the size of the Window.
    void set_window_size(Vec2U size);

    /// Should swap_buffers synchronise with the display?
    void set_vsync_enabled(bool enabled);

    /// Should the cursor be hidden within the Window?
    void set_cursor_hidden(bool hidden);

    /// Should holding a key spawn repeat events?
    void set_key_repeat(bool repeat);

    //--------------------------------------------------------//

    /// Fetch any new events.
    Vector<Event> fetch_events();

    /// Display the newly rendered frame.
    void swap_buffers();

    //--------------------------------------------------------//

    /// Get the current title of the Window.
    String get_window_title() const;

    /// Get the current size of the Window.
    Vec2U get_window_size() const;

    /// Check if display synchronisation is enabled.
    bool get_vsync_enabled() const;

    /// Check if cursor hiding is enabled.
    bool get_cursor_hidden() const;

    /// Check if key repeat is enabled.
    bool get_key_repeat() const;

private: //===================================================//

    struct Implementation;
    friend struct Implementation;
    UniquePtr<Implementation> impl;

    //--------------------------------------------------------//

    friend class InputDevices;
    void* const mSystemWindowPtr;
};

//============================================================================//

} // namespace sq
