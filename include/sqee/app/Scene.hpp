#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq { //###############################################################

struct Event;

//==============================================================================

/// The SQEE Scene base class.
class SQEE_API Scene
{
public: //======================================================

    Scene(double tickTime);

    SQEE_COPY_DELETE(Scene)
    SQEE_MOVE_DELETE(Scene)

    virtual ~Scene() = default;

    //----------------------------------------------------------

    /// Call to update and then integrate the scene.
    void update_and_integrate(double elapsed);

    //----------------------------------------------------------

    /// Optionally implement for event handling.
    virtual void handle_event(Event event);

    /// Optionally implement for refreshing options.
    virtual void refresh_options_destroy();

    /// Optionally implement for refreshing options.
    virtual void refresh_options_create();

    /// Optionally implement for ImGui widgets.
    virtual void show_imgui_widgets();

    //----------------------------------------------------------

    /// Call between cmdbuf.begin() and cmdbuf.end() for drawing commands.
    virtual void populate_command_buffer(vk::CommandBuffer cmdbuf, vk::Framebuffer framebuf) = 0;

protected: //===================================================

    /// Implement to simulate one tick.
    virtual void update() = 0;

    /// Implement to prepare for render.
    virtual void integrate(double elapsed, float blend) = 0;

    //----------------------------------------------------------

    double mTickTime;

    double mAccumulation;

    /// Set just after loading screens to prevent time jumps.
    bool mJustLoaded = true;
};

} // namespace sq ##############################################################
