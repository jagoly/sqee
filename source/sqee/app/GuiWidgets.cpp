#include <sqee/app/GuiWidgets.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <sqee/redist/imgui/imgui_internal.hpp>

namespace sq::gui::detail {

//============================================================================//

struct FramedGroupData { Vec2F beginCursor; int corners; };

static uint8_t framedGroupIndex = 0u;
static FramedGroupData framedGroupArray[256] = {};

//============================================================================//

ScopeResult begin_collapse(const char* label, FlagSet<Collapse> flags)
{
    ScopeResult result { false, false };

    ImGuiWindow* window = imgui::GetCurrentWindow();
    if (window->SkipItems) return result;

    const ImGuiID id = window->GetID(label);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 padding = style.FramePadding;

    const char* labelEnd = imgui::FindRenderedTextEnd(label);
    const ImVec2 labelSize = imgui::CalcTextSize(label, labelEnd, false);

    // We vertically grow up to current line height up the typical widget height.
    const float textBaseOffsetY = maths::max(0.0f, window->DC.CurrentLineTextBaseOffset - padding.y); // Latch before ItemSize changes it
    const float frameHeight = maths::clamp(window->DC.CurrentLineHeight, labelSize.y + padding.y*2, g.FontSize + padding.y*2);

    const float w = imgui::CalcItemWidth();
    const ImRect bb = { window->DC.CursorPos, { window->DC.CursorPos.x + w, window->DC.CursorPos.y + frameHeight } };

    const float textOffsetX = (g.FontSize + padding.x*2); // Collapser arrow width + Spacing
    const float textWidth = g.FontSize + (labelSize.x > 0.0f ? labelSize.x + padding.x*2 : 0.0f); // Include collapser
    imgui::ItemSize(ImVec2(textWidth, frameHeight), textBaseOffsetY);

    result.display = imgui::TreeNodeBehaviorIsOpen(id, 0);

    // early out if header itself is not visible
    if (imgui::ItemAdd(bb, &id) == false) return result;

    ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_NoKeyModifiers;
    if (flags & Collapse::Close) buttonFlags |= ImGuiButtonFlags_AllowOverlapMode;

    bool clicked, hovered, held;
    clicked = imgui::ButtonBehavior(bb, id, &hovered, &held, buttonFlags);

    if (clicked == true)
    {
        result.display = !result.display;
        window->DC.StateStorage->SetInt(id, result.display);
    }

    //--------------------------------------------------------//

    const ImU32 colour = imgui::GetColorU32(hovered ? (held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered) : ImGuiCol_Header);
    const Vec2F textPos = bb.Min + Vec2F(textOffsetX, padding.y + textBaseOffsetY);

    const bool rounded = !result.display || flags & Collapse::NoFrame;
    const auto corners = rounded ? ImGuiCorner_All : ImGuiCorner_TopLeft | ImGuiCorner_TopRight;

    window->DrawList->AddRectFilled(bb.Min, bb.Max, colour, style.FrameRounding, corners);
    imgui::RenderCollapseTriangle(bb.Min + padding + Vec2F(0.0f, textBaseOffsetY), result.display, 1.0f);
    imgui::RenderTextClipped(textPos, bb.Max, label, labelEnd, &labelSize);

    //--------------------------------------------------------//

    if (flags & Collapse::Close)
    {
        imgui::SetItemAllowOverlap();

        const float btnRadius = g.FontSize * 0.5f;
        const float btnPosX = maths::min(window->DC.LastItemRect.Max.x, window->ClipRect.Max.x) - padding.x - btnRadius;
        const float btnPosY = window->DC.LastItemRect.Min.y + g.Style.FramePadding.y + btnRadius;

        const ImGuiID btnID = window->GetID(reinterpret_cast<void*>(intptr_t(id+1)));

        if (imgui::CloseButton(btnID, Vec2F(btnPosX, btnPosY), btnRadius))
            result.close = true;
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

void begin_framed_group(bool lb, bool lt, bool rb, bool rt)
{
    FramedGroupData& data = framedGroupArray[framedGroupIndex++];
    const ImGuiStyle& style = imgui::GetStyle();

    data.beginCursor = imgui::GetCursorScreenPos();

    data.corners = (lb * ImGuiCorner_BottomLeft) | (lt * ImGuiCorner_TopLeft)
                 | (rb * ImGuiCorner_BottomRight) | (rt * ImGuiCorner_TopRight);

    imgui::Indent(style.WindowPadding.x);
    imgui::SetCursorPosY(imgui::GetCursorPosY() + style.WindowPadding.y);
    imgui::PushItemWidth(-(style.WindowPadding.x * float(framedGroupIndex)));
}

void end_framed_group()
{
    SQASSERT(framedGroupIndex != 0u, "");

    const ImGuiStyle& style = get_style();
    FramedGroupData& data = framedGroupArray[--framedGroupIndex];

    imgui::Unindent(style.WindowPadding.x);
    imgui::SetCursorPosY(imgui::GetCursorPosY() + style.WindowPadding.y);
    imgui::PopItemWidth();

    const float rectEndX = data.beginCursor.x + imgui::GetContentRegionAvailWidth() - style.WindowPadding.x * float(framedGroupIndex);
    const float rectEndY = imgui::GetCursorScreenPos().y - style.ItemSpacing.y;

    imgui::GetWindowDrawList()->AddRect(data.beginCursor, {rectEndX, rectEndY}, imgui::GetColorU32(ImGuiCol_Border), style.ChildWindowRounding, data.corners);
}

//============================================================================//

void display_input_label(const char* label, float width)
{
    const ImGuiStyle& style = get_style();

    const float cursorStart = imgui::GetCursorPosX();

    const float textWidth = imgui::CalcTextSize(label).x;
    const float padding = (width - textWidth) / 2.f;

    imgui::AlignFirstTextHeightToWidgets();
    imgui::SetCursorPosX(cursorStart + padding);
    imgui::TextUnformatted(label);
    imgui::SameLine(cursorStart + width + style.ItemSpacing.x);
}

//============================================================================//

bool display_input_text(const char* label, uint maxChars, string& ref, FlagSet<InputText> flags, InputTextCallback callback)
{
    const size_t bufSize = maxChars + 1u;

    //SQASSERT(ref.size() <= maxChars, "");
    if (ref.size() > maxChars) ref.resize(maxChars);

    SQASSERT(callback == nullptr || flags & (InputText::CallbackAlways | InputText::CallbackFilter), "");

    ImGuiInputTextFlags imFlags = 0;

    if (!(flags & InputText::LiveEdit)) imFlags |= ImGuiInputTextFlags_EnterReturnsTrue;
    if (flags & InputText::CallbackAlways) imFlags |= ImGuiInputTextFlags_CallbackAlways;
    if (flags & InputText::CallbackFilter) imFlags |= ImGuiInputTextFlags_CallbackCharFilter;
    if (flags & InputText::DisableTab) imFlags |= ImGuiInputTextFlags_CallbackCompletion;

    // using imgui provided label for now
    //if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    //std::unique_ptr<char[]> buffer { new char[bufSize] };

    auto buffer = std::make_unique<char[]>(bufSize);

    //std::array<char, BufSize> buffer;
    ref.copy(buffer.get(), maxChars);
    buffer[ref.size()] = '\0';

    float height = imgui::CalcTextSize(ref.c_str()).y;
    height += imgui::CalcTextSize("\n").y + get_style().FramePadding.y * 2.f;
    const Vec2F size = { 0.f, height };

    bool changed;

    //--------------------------------------------------------//

    if (callback != nullptr)
    {
        const auto cbWrapper = [](ImGuiTextEditCallbackData* imData) -> int
        {
            InputTextCallbackData data
            {
                string(imData->Buf), uint(imData->CursorPos),
                uint(imData->SelectionStart), uint(imData->SelectionEnd),
                uint32_t(imData->EventChar)
            };

            std::invoke(*static_cast<InputTextCallback*>(imData->UserData), data);

            if (data.text != string_view(imData->Buf))
            {
                const size_t maxChars = size_t(imData->BufSize) - 1u;

                SQASSERT(data.text.size() <= maxChars, "naughty callback!");

                data.text.copy(imData->Buf, maxChars);
                imData->Buf[data.text.size()] = '\0';

                imData->BufTextLen = int(data.text.size());
                imData->BufDirty = true;
            }

            imData->CursorPos = int(data.cursorPos);
            imData->SelectionStart = int(data.selectStart);
            imData->SelectionEnd = int(data.selectEnd);

            imData->EventChar = ImWchar(data.character);

            return 0; // not sure why imgui needs a result
        };

        changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags, cbWrapper, &callback);
    }

    else if (flags & InputText::DisableTab)
    {
        const auto cbDummy = [](ImGuiTextEditCallbackData*) -> int { return 0; };
        changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags, cbDummy);
    }

    else changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags);

    //--------------------------------------------------------//

    // need to do our own string compare when EnterReturnsTrue is enabled
    if (changed && (flags & InputText::LiveEdit || ref != string_view(buffer.get())))
    {
        ref = string(buffer.get());
        return true;
    }

    return false;
}


} // namespace sq::gui::detail
