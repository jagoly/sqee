// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <deque>

#include <sqee/misc/Builtins.hpp>

#include <sqee/app/Scene.hpp>

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

    struct Notification
    {
        String message = "";
        uint timeRemaining = 0u;
    };

    std::deque<Notification> mNotifications;

    double mFrameTime = 0.0;

    bool mActive = true;
};

//============================================================================//

} // namespace sq
