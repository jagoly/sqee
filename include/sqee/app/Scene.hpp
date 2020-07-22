// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/app/Event.hpp>

namespace sq {

//============================================================================//

/// The SQEE Scene base class.
class SQEE_API Scene : private NonCopyable
{
public: //====================================================//

    /// Constructor.
    Scene(double tickTime);

    /// Virtual Destructor.
    virtual ~Scene() = default;

    //--------------------------------------------------------//

    /// Call to update and then render the scene.
    void update_and_render(double elapsed);

    //--------------------------------------------------------//

    /// Optionally implement for event handling.
    virtual void handle_event(Event) {}

    /// Optionally implement for refreshing options.
    virtual void refresh_options() {}

    /// Optionally implement for ImGui widgets.
    virtual void show_imgui_widgets() {}

protected: //=================================================//

    /// Implement to simulate one tick.
    virtual void update() = 0;

    /// Implement to render the scene.
    virtual void render(double elapsed) = 0;

    //--------------------------------------------------------//

    double mTickTime;

    double mAccumulation;
};

//============================================================================//

} // namespace sq
