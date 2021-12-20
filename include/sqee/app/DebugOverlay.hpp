// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/Scene.hpp>
#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

/// The SQEE Debugging Overlay.
class SQEE_API DebugOverlay final : public Scene
{
public: //====================================================//

    DebugOverlay();

    void show_imgui_widgets() override;

    void set_sub_timers(std::initializer_list<const char*> names);
    void update_sub_timers(const double* times);

    /// Display a message in the corner.
    void notify(String message);

    /// Toggle drawing the overlay.
    void toggle_active() { mActive = !mActive; }

    /// Check if the overlay is drawn.
    bool check_active() const { return mActive; }

private: //===================================================//

    void update() override;

    void integrate(double elapsed, float blend) override;

    void populate_command_buffer(vk::CommandBuffer, vk::Framebuffer) override {};

    struct Notification
    {
        uint timeRemaining;
        float width;
        String message;
    };

    std::vector<double> mFrameTimes;
    String mFpsString;
    std::vector<Notification> mNotifications;
    bool mActive = true;

    struct FrameSubTimer
    {
        String name;
        std::vector<double> times;
        String display;
    };

    std::vector<FrameSubTimer> mFrameSubTimers;
    String mFrameSubTimerTotalDisplay;
};

//============================================================================//

} // namespace sq
