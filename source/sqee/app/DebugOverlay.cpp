#include <sqee/messages.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/debug/Text.hpp>

#include <sqee/app/MessageBus.hpp>
#include <sqee/app/DebugOverlay.hpp>

using namespace sq;

//============================================================================//

DebugOverlay::DebugOverlay(Application& app) : mApplication(app) {}

//============================================================================//

void DebugOverlay::tick()
{
    if (mNotifications.empty() == false && --mTicksLeft == 0u)
    {
        mNotifications.pop_front();

        if (mNotifications.empty() == false)
            mTicksLeft = mNotifications.front().second;
    }
}

//============================================================================//

void DebugOverlay::render(float frameTime)
{
    static auto& context = Context::get();

    context.set_ViewPort(mApplication.OPTION_WindowSize);

    mFrameTime = maths::mix(frameTime, mFrameTime, 0.8f);
    const double fps = 1.0 / double(mFrameTime);

    char roundedFPS[10];
    std::sprintf(roundedFPS, "%.2f", fps);

    render_text_basic(roundedFPS, Vec2I(+1, -1), Vec2I(-1, -1), Vec2F(40, 50), Vec4F(1, 1, 1, 1), true);

    if (mNotifications.empty() == false && mTicksLeft != mNotifications.front().second)
    {
        const string& message = mNotifications.front().first;
        const float alpha = mTicksLeft == 1u ? 1.f - accumulation * 8.f : 1.f;

        render_text_basic(message, Vec2I(+1, -1), Vec2I(-1, -1), Vec2F(25, 30), Vec4F(1, 1, 1, alpha), true);
    }
}

//============================================================================//

void DebugOverlay::toggle_active()
{
    mActive = !mActive;

    const msg::Toggle_Debug_Overlay message { mActive };
    mApplication.get_message_bus().send_message(message);
}

//============================================================================//

void DebugOverlay::notify(const string& message, uint ticks)
{
    if (mActive == true)
    {
        mNotifications.emplace_back(message, ticks);
        if (mNotifications.size() == 1u) mTicksLeft = ticks;
    }
}
