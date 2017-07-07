#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/app/Event.hpp>

namespace sq {

//============================================================================//

/// The SQEE Window class.
class Window final : NonCopyable
{
public: //====================================================//

    /// Constructor.
    Window(string title, Vec2U size);

    /// Destructor.
    ~Window();

    //--------------------------------------------------------//

    /// Set the title of the Window.
    void set_window_title(string title);

    /// Set the size of the Window.
    void set_window_size(Vec2U size);

    /// Should swap_buffers synchronise with the display?
    void set_vsync_enabled(bool enabled);

    /// Should the cursor be hidden within the Window?
    void set_cursor_hidden(bool hidden);

    //--------------------------------------------------------//

    /// Fetch any new events.
    std::vector<Event> fetch_events();

    /// Display the newly rendered frame.
    void swap_buffers();

    //--------------------------------------------------------//

    /// Get the current title of the Window.
    string get_window_title() const;

    /// Get the current size of the Window.
    Vec2U get_window_size() const;

    /// Check if display synchronisation is enabled.
    bool get_vsync_enabled() const;

    /// Check if cursor hiding is enabled.
    bool get_cursor_hidden() const;

private: //===================================================//

    struct Implementation;
    friend struct Implementation;
    unique_ptr<Implementation> impl;

    //--------------------------------------------------------//

    friend class InputDevices;
    void* const mSystemWindowPtr;
};

//============================================================================//

} // namespace sq
