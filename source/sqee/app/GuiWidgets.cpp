#include <sqee/app/GuiWidgets.hpp>

#include <sqee/core/Macros.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <dearimgui/imgui_internal.h>
#include <dearimgui/imgui.h>

DISABLE_WARNING_OLD_STYLE_CAST()

//============================================================================//

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

} // ImPlus::detail

bool ImPlus::detail::is_temp_input_open()
{
    return GImGui->TempInputId != 0;
}

//============================================================================//

bool ImGui::DragScalarRange2(const char* label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed, const void* p_min, const void* p_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    ImU64 min_min = 0, min_max = 0, max_min = 0, max_max = 0;

    #define IMGUI_CASE_FOR_DATA_TYPE(Enum, Type, TypeMin, TypeMax) \
    case Enum: { \
        const Type v_min = p_min ? *(const Type*)(p_min) : TypeMin; \
        const Type v_max = p_max ? *(const Type*)(p_max) : TypeMax; \
        const Type v_current_min = *(const Type*)(p_current_min); \
        const Type v_current_max = *(const Type*)(p_current_max); \
        if (v_min >= v_max) { \
            (Type&)min_min = TypeMin; \
            (Type&)min_max = v_current_max; \
            (Type&)max_min = v_current_min; \
            (Type&)max_max = TypeMax; \
        } else { \
            (Type&)min_min = v_min; \
            (Type&)min_max = ImMin(v_max, v_current_max); \
            (Type&)max_min = ImMax(v_min, v_current_min); \
            (Type&)max_max = v_max; \
        } \
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

    ImGuiSliderFlags min_flags = flags | (min_min == min_max ? ImGuiSliderFlags_ReadOnly : 0);
    bool value_changed = DragScalar("##min", data_type, p_current_min, v_speed, &min_min, &min_max, format, min_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    ImGuiSliderFlags max_flags = flags | (max_min == max_max ? ImGuiSliderFlags_ReadOnly : 0);
    value_changed |= DragScalar("##max", data_type, p_current_max, v_speed, &max_min, &max_max, format_max ? format_max : format, max_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}

//============================================================================//

bool ImPlus::InputString(CStrView label, std::string& str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* userData)
{
    flags |= ImGuiInputTextFlags_CallbackResize;

    detail::InputStringCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputTextEx(label, NULL, str.data(), int(str.capacity()) + 1, ImVec2(), flags, detail::input_string_callback, &cbUserData);
}

bool ImPlus::InputStringMultiline(CStrView label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* userData)
{
    flags |= ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_Multiline;

    detail::InputStringCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputTextEx(label, NULL, str.data(), int(str.capacity()) + 1, size, flags, detail::input_string_callback, &cbUserData);
}

//----------------------------------------------------------------------------//

bool ImPlus::InputColour(CStrView label, sq::Vec3F& colour, ImGuiColorEditFlags flags)
{
    sq::Vec3F temp = colour;
    bool changed = ImGui::ColorEdit3(label, temp.data, flags);
    if (changed && GImGui->TempInputId != 0) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) colour = temp;
    return changed;
}

bool ImPlus::InputColour(CStrView label, sq::Vec4F& colour, ImGuiColorEditFlags flags)
{
    sq::Vec4F temp = colour;
    bool changed = ImGui::ColorEdit4(label, temp.data, flags);
    if (changed && GImGui->TempInputId != 0) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) colour = temp;
    return changed;
}

//----------------------------------------------------------------------------//

ImPlus::DialogResult ImPlus::DialogConfirmation(CStrView title, CStrView message)
{
    const ImGuiID id = GImGui->CurrentWindow->GetID(title);
    if (!ImGui::IsPopupOpen(id, ImGuiPopupFlags_None)) ImGui::OpenPopupEx(id);

    DialogResult result = DialogResult::None;

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // ignore key if window just appeared, for when a key press triggered the dialog
        const bool acceptKeyPress = !ImGui::IsWindowAppearing();

        if (message != nullptr)
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

//----------------------------------------------------------------------------//

bool ImPlus::CloseButton(CStrView label)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems == true)
        return false;

    const ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);

    const float frameHeight = ImGui::GetFrameHeight();
    const ImRect bbox = { window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + frameHeight, window->DC.CursorPos.y + frameHeight) };

    ImGui::ItemSize(bbox, 0.0f);
    if (ImGui::ItemAdd(bbox, id) == false)
        return false;

    bool hovered, held;
    const bool pressed = ImGui::ButtonBehavior(bbox, id, &hovered, &held);

    const ImU32 frameColour = ImGui::GetColorU32((held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    ImGui::RenderFrame(bbox.Min, bbox.Max, frameColour, true, g.Style.FrameRounding);

    const ImVec2 centre = bbox.GetCenter();
    const float extent = (frameHeight - g.Style.FramePadding.y) * 0.25f;
    const ImU32 crossColour = ImGui::GetColorU32(ImGuiCol_Text);
    window->DrawList->AddLine({ centre.x + extent, centre.y + extent }, { centre.x - extent, centre.y - extent }, crossColour);
    window->DrawList->AddLine({ centre.x + extent, centre.y - extent }, { centre.x - extent, centre.y + extent }, crossColour);

    return pressed;
}

//----------------------------------------------------------------------------//

void ImPlus::TextColored(ImVec4 col, std::string_view text)
{
    ImGui::PushStyleColor(ImGuiCol_Text, col);
    ImGui::TextUnformatted(text.data(), text.data() + text.length());
    ImGui::PopStyleColor();
}

void ImPlus::TextDisabled(std::string_view text)
{
    ImGui::PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
    ImGui::TextUnformatted(text.data(), text.data() + text.length());
    ImGui::PopStyleColor();
}

void ImPlus::TextWrapped(std::string_view text)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    bool need_backup = (window->DC.TextWrapPos < 0.0f);
    if (need_backup)
        ImGui::PushTextWrapPos(0.0f);
    ImGui::TextUnformatted(text.data(), text.data() + text.length());
    if (need_backup)
        ImGui::PopTextWrapPos();
}

//----------------------------------------------------------------------------//

void ImPlus::LabelText(CStrView label, std::string_view text)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float w = ImGui::CalcItemWidth();

    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, 0))
        return;

    ImGui::RenderTextClipped(value_bb.Min, value_bb.Max, text.data(), text.data() + text.length(), nullptr, ImVec2(0.0f, 0.5f));
    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void ImPlus::BulletText(std::string_view text)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = CalcTextSize(text.data(), text.data() + text.length(), false);
    const ImVec2 total_size = ImVec2(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ImGui::ItemSize(total_size, 0.0f);
    const ImRect bb(pos, pos + total_size);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
    ImGui::RenderBullet(window->DrawList, bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, g.FontSize * 0.5f), text_col);
    ImGui::RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, 0.0f), text.data(), text.data() + text.length(), false);
}

void ImPlus::SetTooltip(std::string_view text)
{
    ImGui::BeginTooltipEx(0, ImGuiTooltipFlags_OverridePreviousTooltip);
    ImGui::TextUnformatted(text.data(), text.data() + text.length());
    ImGui::EndTooltip();
}
