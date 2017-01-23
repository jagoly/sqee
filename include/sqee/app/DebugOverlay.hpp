#pragma once

#include <deque>

#include <sqee/app/Application.hpp>

namespace sq {

//============================================================================//

/// The SQEE Debugging Overlay
class DebugOverlay final : NonCopyable
{
public:

    //========================================================//

    /// Constructor
    DebugOverlay(Application& app);

    //========================================================//

    /// called eight times per second
    void tick();

    /// called once every frame
    void render(float frameTime);

    //========================================================//

    /// enable or disable the overlay
    void toggle_active();

    /// check if the overlay is active
    bool is_active() const { return mActive; }

    //========================================================//

    /// display a message in the corner
    void notify(const string& message, uint ticks);

    //========================================================//

    float accumulation = 0.0f;

private:

    //========================================================//

    Application& mApplication;

    std::deque<pair<string, uint>> mNotifications;

    float mFrameTime = 0.f;
    uint mTicksLeft = 0u;

    bool mActive = true;
};

//============================================================================//

} // namespace sq
