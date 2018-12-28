// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/DebugOverlay.hpp>

#include <sqee/debug/Text.hpp>

#include <algorithm>
#include <numeric>

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

    mFrameTimes[mFrameTimeIndex] = elapsed;

    if (++mFrameTimeIndex == 60u) mFrameTimeIndex = 0u;

    //--------------------------------------------------------//

    if (mActive == true)
    {
        char fpsString[8]; std::snprintf(fpsString, 8, "%.2f", 1.0 / mFrameTime);

        // display frame rate in the bottom left corner
        render_text_basic(fpsString, {+1, +1}, {-1, -1}, {40.f, 50.f}, {1.f, 1.f, 1.f, 1.f}, true);

//        const auto [min, max] = std::minmax_element(mFrameTimes.begin(), mFrameTimes.end());

//        const double average = std::accumulate(mFrameTimes.begin(), mFrameTimes.end(), 0.0) / 60.0;

//        char ftString[48];
//        std::snprintf(ftString, 48, "fps: %0.4f\nmin: %0.5fms\nmax: %0.5fms", 1.0/average, *min*1000.0, *max*1000.0);

//        render_text_basic(ftString, {+1, -1}, {-1, -1}, {32.f, 32.f}, {1.f, 1.f, 1.f, 1.f}, true);

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
        if (mNotifications.size() >= 10u) mNotifications.pop_front();

        mNotifications.push_back({message, 20u});
    }
}
