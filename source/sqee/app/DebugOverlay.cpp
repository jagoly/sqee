#include <sqee/app/DebugOverlay.hpp>

#include <sqee/debug/Text.hpp>

#include <fmt/core.h>

using namespace sq;

//============================================================================//

DebugOverlay::DebugOverlay() : Scene(0.1) {}

//============================================================================//

void DebugOverlay::update()
{
    for (auto iter = mNotifications.begin(); iter != mNotifications.end();)
    {
        if (--iter->timeRemaining == 0u)
            iter = mNotifications.erase(iter);
        else ++iter;
    }
}

//============================================================================//

void DebugOverlay::render(double elapsed)
{
    mFrameTime = maths::mix(mFrameTime, elapsed, 0.2);

    //--------------------------------------------------------//

    mFrameTimes[mFrameTimeIndex] = elapsed;

    if (++mFrameTimeIndex == 60u)
        mFrameTimeIndex = 0u;

    //--------------------------------------------------------//

    if (mActive == true)
    {
        const String fpsString = fmt::format("{:.2f}", 1.0 / mFrameTime);

        // display frame rate in the bottom left corner
        render_text_basic(fpsString, {+1, +1}, {-1, -1}, {40.f, 50.f}, {1.f, 1.f, 1.f, 1.f}, true);

        //--------------------------------------------------------//

        auto lineNumber = mNotifications.size();

        for (const auto& [message, timeRemaining] : mNotifications)
        {
            const auto offsetMessage = String(--lineNumber, '\n') + message;
            const float alpha = timeRemaining == 1u ? 1.f - float(mAccumulation) * 8.f : 1.f;

            // display notifications in the bottom right corner
            render_text_basic(offsetMessage, {+1, +1}, {+1, -1}, {25.f, 30.f}, {1.f, 1.f, 1.f, alpha}, true);
        }
    }
}

//============================================================================//

void DebugOverlay::notify(String message)
{
    if (mActive == true)
    {
        mNotifications.emplace_back();
        mNotifications.back().message = std::move(message);
        mNotifications.back().timeRemaining = 20u;
    }
}
