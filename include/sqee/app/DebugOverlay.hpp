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

    /// Constructor.
    DebugOverlay();

    //--------------------------------------------------------//

    /// Toggle the overlay.
    void toggle_active() { mActive = !mActive; }

    /// Check if the overlay is active,
    bool check_active() const { return mActive; }

    //--------------------------------------------------------//

    /// Display a message in the corner.
    void notify(String message);

private: //===================================================//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    struct Notification : MoveOnly
    {
        String message = "";
        uint timeRemaining = 0u;
    };

    std::vector<Notification> mNotifications;

    double mFrameTime = 0.0;

    size_t mFrameTimeIndex = 0u;
    std::array<double, 60u> mFrameTimes;

    bool mActive = true;
};

//============================================================================//

} // namespace sq
