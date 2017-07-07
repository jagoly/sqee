#include <sqee/debug/Text.hpp>

#include <sqee/app/DebugOverlay.hpp>

using namespace sq;

//============================================================================//

DebugOverlay::DebugOverlay() : Scene(0.1) {}

//============================================================================//

void DebugOverlay::update()
{
    for (auto& notifcation : mNotifications)
        --notifcation.timeRemaining;

    for (auto& notifcation : mNotifications)
    {
        if (notifcation.timeRemaining != 0u) break;
        else mNotifications.pop_front();
    }
}

//============================================================================//

void DebugOverlay::render(double elapsed)
{
    mFrameTime = maths::mix(mFrameTime, elapsed, 0.2);

    //--------------------------------------------------------//

    if (mActive == true)
    {
        char fpsString[8]; std::snprintf(fpsString, 8, "%.2f", 1.0 / mFrameTime);

        // display frame rate in the bottom left corner
        render_text_basic(fpsString, {+1, +1}, {-1, -1}, {40.f, 50.f}, {1.f, 1.f, 1.f, 1.f}, true);

        //--------------------------------------------------------//

        auto lineNumber = mNotifications.size();

        for (const auto& [message, timeRemaining] : mNotifications)
        {
            const string offsetMessage = string(--lineNumber, '\n') + message;
            const float alpha = timeRemaining == 1u ? 1.f - float(mAccumulation) * 8.f : 1.f;

            // display notifications in the bottom right corner
            render_text_basic(offsetMessage, {+1, +1}, {+1, -1}, {25.f, 30.f}, {1.f, 1.f, 1.f, alpha}, true);
        }
    }
}

//============================================================================//

void DebugOverlay::notify(const string& message)
{
    if (mActive == true)
    {
        if (mNotifications.size() >= 10u) mNotifications.pop_front();

        mNotifications.push_back({message, 20u});
    }
}
