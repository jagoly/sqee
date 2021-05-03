#include <sqee/app/DebugOverlay.hpp>

#include <sqee/app/GuiWidgets.hpp>
#include <sqee/core/Algorithms.hpp>

using namespace sq;

//============================================================================//

DebugOverlay::DebugOverlay() : Scene(1.0 / 4.0) {}

//============================================================================//

void DebugOverlay::update()
{
    for (auto iter = mNotifications.begin(); iter != mNotifications.end();)
    {
        if (--iter->timeRemaining == 0u)
            iter = mNotifications.erase(iter);
        else ++iter;
    }

    if (mFrameTimes.empty() == false)
    {
        const double totalTime = std::accumulate(mFrameTimes.begin(), mFrameTimes.end(), 0.0);
        mFpsString = fmt::format("{:.0f}", double(mFrameTimes.size()) / totalTime);
        mFrameTimes.clear();
    }
    else mFpsString = "0";
}

//============================================================================//

void DebugOverlay::integrate(double elapsed, float /*blend*/)
{
    mFrameTimes.push_back(elapsed);
}

//============================================================================//

void DebugOverlay::show_imgui_widgets()
{
    if (mActive == false)
        return;

    const ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImFont* fontRegular = ImGui::GetIO().Fonts->Fonts[ImPlus::FONT_REGULAR];
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    // show FPS display in bottom left corner
    {
        fontRegular->Scale = 3.f;
        const ImPlus::ScopeFont font = fontRegular;

        const char* fpsEnd = mFpsString.data() + mFpsString.length();

        // inefficent but simple way to do text outline, fine for just a few characters
        drawList->AddText(ImVec2(14, ImPlus::FromScreenBottom(58)), IM_COL32_BLACK, mFpsString.data(), fpsEnd);
        drawList->AddText(ImVec2(14, ImPlus::FromScreenBottom(62)), IM_COL32_BLACK, mFpsString.data(), fpsEnd);
        drawList->AddText(ImVec2(18, ImPlus::FromScreenBottom(58)), IM_COL32_BLACK, mFpsString.data(), fpsEnd);
        drawList->AddText(ImVec2(18, ImPlus::FromScreenBottom(62)), IM_COL32_BLACK, mFpsString.data(), fpsEnd);
        drawList->AddText(ImVec2(16, ImPlus::FromScreenBottom(60)), IM_COL32_WHITE, mFpsString.data(), fpsEnd);

        fontRegular->Scale = 1.f;
    }

    // show notification window in bottom right corner
    if (mNotifications.empty() == false)
    {
        fontRegular->Scale = 2.f;
        const ImPlus::ScopeFont font = fontRegular;

        float maxWidth = 0.f;
        for (const auto& item : mNotifications)
            maxWidth = std::max(maxWidth, item.width);

        const float rectLeft = displaySize.x - 20.f - maxWidth;
        const float rectTop = displaySize.y - 12.f - 32.f * float(mNotifications.size());
        drawList->AddRectFilled({rectLeft, rectTop}, displaySize, ImColor(style.Colors[ImGuiCol_WindowBg]));

        for (size_t lineNum = 0u; lineNum < mNotifications.size(); ++lineNum)
        {
            const auto& [timeRemaining, width, message] = mNotifications[lineNum];

            const float x = displaySize.x - 10.f - width;
            const float y = displaySize.y - 8.f - 32.f * float(mNotifications.size() - lineNum);
            const char* messageEnd = message.data() + message.length();

            drawList->AddText(ImVec2(x, y), IM_COL32_WHITE, message.data(), messageEnd);
        }

        fontRegular->Scale = 1.f;
    }
}

//============================================================================//

void DebugOverlay::notify(String message)
{
    if (mActive == true)
    {
        Notification& entry = mNotifications.emplace_back();

        entry.timeRemaining = 8u;
        entry.width = ImGui::CalcTextSize(message.data(), message.data() + message.length()).x * 2.f;
        entry.message = std::move(message);
    }
}
