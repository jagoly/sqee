#include <sqee/app/GuiWidgets.hpp>

#include <sqee/core/Macros.hpp>
#include <sqee/maths/Vectors.hpp>

#include <dearimgui/imgui_internal.h>

DISABLE_WARNING_OLD_STYLE_CAST()

//==============================================================================

namespace ImPlus::detail {

struct InputStringCallbackUserData
{
    std::string& str;
    ImGuiInputTextCallback chainCallback;
    void* chainCallbackUserData;
};

int input_string_callback(ImGuiInputTextCallbackData* data)
{
    auto userData = static_cast<InputStringCallbackUserData*>(data->UserData);
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        IM_ASSERT(data->Buf == userData->str.data());
        userData->str.resize(static_cast<size_t>(data->BufTextLen));
        data->Buf = userData->str.data();
    }
    else if (userData->chainCallback != nullptr)
    {
        data->UserData = userData->chainCallbackUserData;
        return userData->chainCallback(data);
    }
    return 0;
}

// maximum number of simultaneous lazy format strings
constexpr int NUM_FORMAT_BUFFERS = 2;

thread_local std::array<fmt::memory_buffer, NUM_FORMAT_BUFFERS> gFormatBuffers;
thread_local int gFormatBufferIndex = -1;

struct LazyFormat
{
    std::string_view sv; int bufferIndex = -1;

    LazyFormat(const fmt::string_view& fstr, const fmt::format_args& args)
    {
        // check if there are any arguments by checking if desc_ has been set
        if (reinterpret_cast<const unsigned long long&>(args) != 0)
        {
            bufferIndex = ++gFormatBufferIndex;
            assert(bufferIndex >= 0 && bufferIndex < NUM_FORMAT_BUFFERS);

            fmt::memory_buffer& buffer = gFormatBuffers[bufferIndex];

            fmt::vformat_to(fmt::appender(buffer), fstr, args);
            sv = std::string_view(buffer.data(), buffer.size());
        }
        else sv = std::string_view(fstr.data(), fstr.size());
    }

    LazyFormat(const LazyFormat&) = delete;
    LazyFormat& operator=(const LazyFormat&) = delete;
    LazyFormat(LazyFormat&&) = delete;
    LazyFormat& operator=(LazyFormat&&) = delete;

    ~LazyFormat()
    {
        if (bufferIndex != -1)
        {
            assert(bufferIndex == gFormatBufferIndex);
            gFormatBuffers[bufferIndex] = fmt::memory_buffer();
            --gFormatBufferIndex;
        }
    }

    inline operator std::string_view() const { return sv; }
    inline operator ImStrv() const { return ImStrv(sv.data(), sv.data() + sv.size()); }
};

} // namespace ImPlus::detail

bool ImPlus::detail::is_temp_input_open()
{
    return GImGui->TempInputId != 0;
}

//==============================================================================

bool ImGui::DragScalarRange2(ImStrv label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed, const void* p_min, const void* p_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();

    bool value_changed = false;

    #define IMGUI_CASE_FOR_DATA_TYPE(Enum, Type, TypeMin, TypeMax) \
    case Enum: { \
        Type min_min = 0, min_max = 0, max_min = 0, max_max = 0; \
        const Type v_min = p_min ? *(const Type*)(p_min) : TypeMin; \
        const Type v_max = p_max ? *(const Type*)(p_max) : TypeMax; \
        const Type v_current_min = *(const Type*)(p_current_min); \
        const Type v_current_max = *(const Type*)(p_current_max); \
        if (v_min >= v_max) { \
            min_min = TypeMin; \
            min_max = v_current_max; \
            max_min = v_current_min; \
            max_max = TypeMax; \
        } else { \
            min_min = v_min; \
            min_max = ImMin(v_max, v_current_max); \
            max_min = ImMax(v_min, v_current_min); \
            max_max = v_max; \
        } \
        PushMultiItemsWidths(2, CalcItemWidth()); \
        ImGuiSliderFlags min_flags = flags | (min_min == min_max ? ImGuiSliderFlags_ReadOnly : 0); \
        value_changed |= DragScalar("##min", data_type, p_current_min, v_speed, &min_min, &min_max, format, min_flags); \
        PopItemWidth(); \
        SameLine(0, g.Style.ItemInnerSpacing.x); \
        ImGuiSliderFlags max_flags = flags | (max_min == max_max ? ImGuiSliderFlags_ReadOnly : 0); \
        value_changed |= DragScalar("##max", data_type, p_current_max, v_speed, &max_min, &max_max, format_max ? format_max : format, max_flags); \
        PopItemWidth(); \
        SameLine(0, g.Style.ItemInnerSpacing.x); \
        break; \
    }

    switch (data_type)
    {
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_S8,     ImS8,   INT8_MIN,  INT8_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_U8,     ImU8,   0u,        UINT8_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_S16,    ImS16,  INT16_MIN, INT16_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_U16,    ImU16,  0u,        UINT16_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_S32,    ImS32,  INT32_MIN, INT32_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_U32,    ImU32,  0u,        UINT32_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_S64,    ImS64,  INT64_MIN, INT64_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_U64,    ImU64,  0u,        UINT64_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_Float,  float,  -FLT_MAX,  FLT_MAX)
        IMGUI_CASE_FOR_DATA_TYPE(ImGuiDataType_Double, double, -DBL_MAX,  DBL_MAX)
        default: IM_ASSERT(0);
    }

    #undef IMGUI_CASE_FOR_DATA_TYPE

    label.End = FindRenderedTextEnd(label);
    TextEx(label);
    EndGroup();
    PopID();

    return value_changed;
}

//==============================================================================

float ImPlus::FromScreenRight(float offset)
{
    return GImGui->IO.DisplaySize.x - offset;
}

float ImPlus::FromScreenBottom(float offset)
{
    return GImGui->IO.DisplaySize.y - offset;
}

void ImPlus::PushFont(int index)
{
    ImGui::PushFont(GImGui->IO.Fonts->Fonts[index]);
}

void ImPlus::AutoArrange(float minItemWidth)
{
    const ImGuiWindow& window = *GImGui->CurrentWindow;

    const float contentWidth = window.ContentRegionRect.GetWidth() - 1.f;
    const float itemWidth = contentWidth / ImMax(ImFloor(contentWidth / minItemWidth), 1.f);
    const float prevItemMin = GImGui->LastItemData.Rect.Min.x;

    if (prevItemMin + itemWidth * 2.f < window.ContentRegionRect.Max.x)
        ImGui::SameLine(prevItemMin + itemWidth - window.Pos.x);
}

//------------------------------------------------------------------------------

bool ImPlus::InputString(ImStrv label, std::string& str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* userData)
{
    flags |= ImGuiInputTextFlags_CallbackResize;

    detail::InputStringCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputTextEx(label, ImStrv(), str.data(), int(str.capacity()) + 1, ImVec2(), flags, detail::input_string_callback, &cbUserData);
}

bool ImPlus::InputStringMultiline(ImStrv label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* userData)
{
    flags |= ImGuiInputTextFlags_CallbackResize | int(ImGuiInputTextFlags_Multiline);

    detail::InputStringCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputTextEx(label, ImStrv(), str.data(), int(str.capacity()) + 1, size, flags, detail::input_string_callback, &cbUserData);
}

//------------------------------------------------------------------------------

bool ImPlus::InputColour(ImStrv label, sq::Vec3F& colour, ImGuiColorEditFlags flags)
{
    sq::Vec3F temp = colour;
    bool changed = ImGui::ColorEdit3(label, temp.data(), flags);
    if (changed && GImGui->TempInputId != 0) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) colour = temp;
    return changed;
}

bool ImPlus::InputColour(ImStrv label, sq::Vec4F& colour, ImGuiColorEditFlags flags)
{
    sq::Vec4F temp = colour;
    bool changed = ImGui::ColorEdit4(label, temp.data(), flags);
    if (changed && GImGui->TempInputId != 0) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) colour = temp;
    return changed;
}

//------------------------------------------------------------------------------

ImPlus::DialogResult ImPlus::DialogConfirmation(ImStrv title, std::string_view message)
{
    const ImGuiID id = GImGui->CurrentWindow->GetID(title);
    if (!ImGui::IsPopupOpen(id, ImGuiPopupFlags_None)) ImGui::OpenPopupEx(id);

    DialogResult result = DialogResult::None;

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // ignore key if window just appeared, for when a key press triggered the dialog
        const bool acceptKeyPress = !ImGui::IsWindowAppearing();

        if (!message.empty())
            ImGui::TextUnformatted(message);

        if (ImGui::Button("Confirm") || (acceptKeyPress && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false)))
        {
            result = DialogResult::Confirm;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel") || (acceptKeyPress && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape), false)))
        {
            result = DialogResult::Cancel;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else result = DialogResult::Cancel;

    return result;
}

//------------------------------------------------------------------------------

void ImPlus::HoverTooltipV(bool delay, ImGuiDir dir, fmt::string_view fstr, fmt::format_args args)
{
    if (!ImGui::IsItemHovered(delay ? ImGuiHoveredFlags_ForTooltip : 0)) return;

    const auto text = detail::LazyFormat(fstr, args);

    if (dir != ImGuiDir_None)
    {
        const ImGuiStyle& style = GImGui->Style;

        const ImVec2 tooltipSize = ImGui::CalcTextSize(text) + style.WindowPadding * 2.f;
        const ImVec2 itemMin = ImGui::GetItemRectMin();
        const ImVec2 itemMax = ImGui::GetItemRectMax();

        ImVec2 tooltipPos = ImFloor((itemMin + itemMax - tooltipSize) * 0.5f);
        if (dir == ImGuiDir_Left)
            tooltipPos.x = itemMin.x - tooltipSize.x - style.ItemSpacing.x;
        if (dir == ImGuiDir_Right)
            tooltipPos.x = itemMax.x + style.ItemSpacing.x;
        if (dir == ImGuiDir_Up)
            tooltipPos.y = itemMin.y - tooltipSize.y - style.ItemSpacing.y;
        if (dir == ImGuiDir_Down)
            tooltipPos.y = itemMax.y + style.ItemSpacing.y;
        tooltipPos = ImClamp(tooltipPos, style.DisplaySafeAreaPadding, GImGui->IO.DisplaySize - tooltipSize - style.DisplaySafeAreaPadding);

        ImGui::SetNextWindowPos(tooltipPos);
    }

    ImPlus::SetTooltip(text);
}

//------------------------------------------------------------------------------

// These are exact copies of the ImGui versions of these functions, but taking
// std::string_views instead of a c format string and va_args. This is to allow
// strings to be formatted with fmtlib or std::format.

using namespace ImGui;

void ImPlus::TextColored(const ImVec4& col, std::string_view text)
{
    PushStyleColor(ImGuiCol_Text, col);
    TextUnformatted(text);
    PopStyleColor();
}

void ImPlus::TextDisabled(std::string_view text)
{
    PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
    TextUnformatted(text);
    PopStyleColor();
}

void ImPlus::TextWrapped(std::string_view text)
{
    const bool need_backup = (GImGui->CurrentWindow->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
    if (need_backup)
        PushTextWrapPos(0.0f);
    TextUnformatted(text);
    if (need_backup)
        PopTextWrapPos();
}

void ImPlus::LabelText(ImStrv label, std::string_view text)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = GImGui->Style;
    const float w = CalcItemWidth();

    const ImVec2 value_size = CalcTextSize(text, false);
    const ImVec2 label_size = CalcTextSize(label, true);
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect value_bb(pos, pos + ImVec2(w, value_size.y + style.FramePadding.y * 2));
    const ImRect total_bb(pos, pos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), ImMax(value_size.y, label_size.y) + style.FramePadding.y * 2));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0))
        return;

    // Render
    RenderTextClipped(value_bb.Min + style.FramePadding, value_bb.Max, text, &value_size, ImVec2(0.0f, 0.0f));
    if (label_size.x > 0.0f)
        RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void ImPlus::BulletText(std::string_view text)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = CalcTextSize(text, false);
    const ImVec2 total_size = ImVec2(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), label_size.y);  // Empty text doesn't add padding
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ItemSize(total_size, 0.0f);
    const ImRect bb(pos, pos + total_size);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    ImU32 text_col = GetColorU32(ImGuiCol_Text);
    RenderBullet(window->DrawList, bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, g.FontSize * 0.5f), text_col);
    RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, 0.0f), text, false);
}

void ImPlus::SetTooltip(std::string_view text)
{
    if (!BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None))
        return;
    TextUnformatted(text);
    EndTooltip();
}
