// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq {

//====== Forward Declarations ================================================//

struct Event;

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

    /// Call to update and then integrate the scene.
    void update_and_integrate(double elapsed);

    //--------------------------------------------------------//

    /// Optionally implement for event handling.
    virtual void handle_event(Event event);

    /// Optionally implement for refreshing options.
    virtual void refresh_options_destroy();

    /// Optionally implement for refreshing options.
    virtual void refresh_options_create();

    /// Optionally implement for ImGui widgets.
    virtual void show_imgui_widgets();

    //--------------------------------------------------------//

    /// Call between cmdbuf.begin() and cmdbuf.end() for drawing commands.
    //virtual void populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf) = 0;
    virtual void populate_command_buffer(vk::CommandBuffer /*cmdbuf*/, vk::Framebuffer /*framebuf*/) {}

protected: //=================================================//

    /// Implement to simulate one tick.
    virtual void update() = 0;

    /// Implement to prepare for render.
    //virtual void integrate(double elapsed, float blend) = 0;
    virtual void integrate(double /*elapsed*/, float /*blend*/) {}

    /// Implement to render the scene.
    virtual void render(double /*elapsed*/) {}

    //--------------------------------------------------------//

    double mTickTime;

    double mAccumulation;
};

//============================================================================//

} // namespace sq
