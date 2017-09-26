#include <sqee/app/GuiWidgets.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <sqee/redist/imgui/imgui_internal.hpp>

namespace sq::gui::detail {

//============================================================================//

struct FramedGroupData { Vec2F beginCursor; int corners; };

static uint8_t framedGroupIndex = 0u;
static FramedGroupData framedGroupArray[256] = {};

//============================================================================//

bool CollapsingHeaderSized(const char* label)
{
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(label);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 padding = style.FramePadding;

    const char* label_end = FindRenderedTextEnd(label);
    const ImVec2 label_size = CalcTextSize(label, label_end, false);

    // We vertically grow up to current line height up the typical widget height.
    const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset - padding.y); // Latch before ItemSize changes it
    const float frame_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + style.FramePadding.y*2), label_size.y + padding.y*2);

    const float w = CalcItemWidth();
    ImRect bb = ImRect(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + frame_height));

    const float text_offset_x = (g.FontSize + padding.x*2); // Collapser arrow width + Spacing
    const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x*2 : 0.0f); // Include collapser
    ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);

    bool is_open = TreeNodeBehaviorIsOpen(id, 0);
    if (!ItemAdd(bb, &id))
    {
        return is_open;
    }

    ImGuiButtonFlags button_flags = ImGuiButtonFlags_NoKeyModifiers;

    bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
    if (pressed)
    {
        is_open = !is_open;
        window->DC.StateStorage->SetInt(id, is_open);
    }

    // Render
    const ImU32 col = GetColorU32(hovered ? (held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered) : ImGuiCol_Header);
    const ImVec2 text_pos = bb.Min + ImVec2(text_offset_x, padding.y + text_base_offset_y);

    // modified RenderFrame
    if (is_open == false) window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImGuiCorner_All);
    else window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, ImGuiCorner_TopLeft | ImGuiCorner_TopRight);
    // todo: add border support

    //RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
    RenderCollapseTriangle(bb.Min + padding + ImVec2(0.0f, text_base_offset_y), is_open, 1.0f);

    RenderTextClipped(text_pos, bb.Max, label, label_end, &label_size);

    return is_open;
}

void BeginFramedGroup(bool lb, bool lt, bool rb, bool rt)
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

void EndFramedGroup()
{
    SQASSERT(framedGroupIndex != 0u, "");

    const ImGuiStyle& style = imgui::GetStyle();
    FramedGroupData& data = framedGroupArray[--framedGroupIndex];

    imgui::Unindent(style.WindowPadding.x);
    imgui::SetCursorPosY(imgui::GetCursorPosY() + style.WindowPadding.y);
    imgui::PopItemWidth();

    const float rectEndX = data.beginCursor.x + imgui::GetContentRegionAvailWidth() - style.WindowPadding.x * float(framedGroupIndex);
    const float rectEndY = imgui::GetCursorScreenPos().y - style.ItemSpacing.y;

    imgui::GetWindowDrawList()->AddRect(data.beginCursor, {rectEndX, rectEndY}, imgui::GetColorU32(ImGuiCol_Border), style.ChildWindowRounding, data.corners);
}

//============================================================================//

} // namespace sq::gui::detail
