#include <sqee/app/GuiWidgets.hpp>

#include <sqee/core/Macros.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <dearimgui/imgui_internal.h>
#include <dearimgui/imgui.h>

DISABLE_WARNING_OLD_STYLE_CAST()

//============================================================================//

namespace { // anonymous

template <class Type>
constexpr ImGuiDataType_ impl_get_data_type()
{
    if (std::is_same_v<Type, int8_t>)   return ImGuiDataType_S8;
    if (std::is_same_v<Type, uint8_t>)  return ImGuiDataType_U8;
    if (std::is_same_v<Type, int16_t>)  return ImGuiDataType_S16;
    if (std::is_same_v<Type, uint16_t>) return ImGuiDataType_U16;
    if (std::is_same_v<Type, int32_t>)  return ImGuiDataType_S32;
    if (std::is_same_v<Type, uint32_t>) return ImGuiDataType_U32;
    if (std::is_same_v<Type, int64_t>)  return ImGuiDataType_S64;
    if (std::is_same_v<Type, uint64_t>) return ImGuiDataType_U64;
    if (std::is_same_v<Type, float>)    return ImGuiDataType_Float;
    if (std::is_same_v<Type, double>)   return ImGuiDataType_Double;
}

struct impl_InputTextCallbackUserData
{
    std::string& str;
    ImGuiInputTextCallback chainCallback;
    void* chainCallbackUserData;
};

int impl_input_text_callback(ImGuiInputTextCallbackData* data)
{
    auto userData = static_cast<impl_InputTextCallbackUserData*>(data->UserData);
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

union impl_UnionValue
{
    int8_t s8; uint8_t u8;
    int16_t s16; uint16_t u16;
    int32_t s32; uint32_t u32;
    int64_t s64; uint64_t u64;
    float f; double d;
};

std::array<impl_UnionValue, 4> impl_scalar_range_2_get_ranges(ImGuiDataType data_type, const void* current_min, const void* current_max, const void* min, const void* max)
{
    std::array<impl_UnionValue, 4> result;

    // (v_min >= v_max) ? -FLT_MAX : v_min
    // (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max)
    // (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min)
    // (v_min >= v_max) ? FLT_MAX : v_max

    #define CASE_FOR_DATA_TYPE(Enum, Type, Member, TypeMin, TypeMax) \
    if (data_type == Enum) { \
        if (*(const Type*)(min) >= *(const Type*)(max)) { \
            result[0].Member = TypeMin; \
            result[1].Member = *(const Type*)(current_max); \
            result[2].Member = *(const Type*)(current_min); \
            result[3].Member = TypeMax; \
        } else { \
            result[0].Member = *(const Type*)(min); \
            result[1].Member = ImMin(*(const Type*)(max), *(const Type*)(current_max)); \
            result[2].Member = ImMax(*(const Type*)(min), *(const Type*)(current_min)); \
            result[3].Member = *(const Type*)(max); \
        } \
    }

    CASE_FOR_DATA_TYPE(ImGuiDataType_S8, int8_t, s8, SCHAR_MIN, SCHAR_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_U8, uint8_t, u8, 0u, UCHAR_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_S16, int16_t, s16, SHRT_MIN, SHRT_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_U16, uint16_t, u16, 0u, USHRT_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_S32, int32_t, s32, INT_MIN, INT_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_U32, uint32_t, u32, 0u, UINT_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_S64, int64_t, s64, LONG_MIN, LONG_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_U64, uint64_t, u64, 0u, ULONG_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_Float, float, f, -FLT_MAX, FLT_MAX);
    CASE_FOR_DATA_TYPE(ImGuiDataType_Double, double, d, -DBL_MAX, DBL_MAX);

    #undef CASE_FOR_DATA_TYPE

    return result;
}

template <class Type>
static bool impl_InputValue(ImPlus::CStrView label, Type& ref, Type step, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    ImGui::InputScalar(label, dataType, &temp, step > 0 ? &step : nullptr, nullptr, format);
    const bool changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
static bool impl_DragValue(ImPlus::CStrView label, Type& ref, Type min, Type max, float speed, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    const bool tempInputTextActive = GImGui->TempInputId != 0 && ImGui::TempInputIsActive(GImGui->CurrentWindow->GetID(label));
    bool changed = ImGui::DragScalar(label, dataType, &temp, speed, &min, &max, format);
    if (tempInputTextActive) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = sq::maths::clamp(temp, min, max);
    return changed;
}

template <class Type>
static bool impl_SliderValue(ImPlus::CStrView label, Type& ref, Type min, Type max, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    const bool tempInputActive = GImGui->TempInputId != 0 && ImGui::TempInputIsActive(GImGui->CurrentWindow->GetID(label));
    bool changed = ImGui::SliderScalar(label, dataType, &temp, &min, &max, format);
    if (tempInputActive) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = sq::maths::clamp(temp, min, max);
    return changed;
}

template <class Type>
static bool impl_InputValueRange2(ImPlus::CStrView label, Type& refMin, Type& refMax, Type step, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    ImGui::InputScalarN(label, dataType, temp, 2, step > 0 ? &step : nullptr, nullptr, format);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        const bool overlap = temp[0] > temp[1];
        if (overlap && temp[0] != refMin) temp[1] = temp[0];
        else if (overlap && temp[1] != refMax) temp[0] = temp[1];
        refMin = temp[0]; refMax = temp[1];
        return true;
    }
    return false;
}

template <class Type>
static bool impl_DragValueRange2(ImPlus::CStrView label, Type& refMin, Type& refMax, Type min, Type max, float speed, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    const bool changed = ImGui::DragScalarN(label, dataType, temp, 2, speed, &min, &max, format);
    if (changed && (GImGui->TempInputId == 0 || ImGui::IsItemDeactivatedAfterEdit()))
    {
        const bool overlap = temp[0] > temp[1];
        if (overlap && temp[0] != refMin) temp[0] = temp[1];
        else if (overlap && temp[1] != refMax) temp[1] = temp[0];
        refMin = sq::maths::clamp(temp[0], min, max);
        refMax = sq::maths::clamp(temp[1], min, max);
        return true;
    }
    return false;
}

template <class Type>
static bool impl_SliderValueRange2(ImPlus::CStrView label, Type& refMin, Type& refMax, Type min, Type max, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    const bool changed = ImGui::SliderScalarN(label, dataType, temp, 2, &min, &max, format);
    if (changed && (GImGui->TempInputId == 0 || ImGui::IsItemDeactivatedAfterEdit()))
    {
        const bool overlap = temp[0] > temp[1];
        if (overlap && temp[0] != refMin) temp[0] = temp[1];
        else if (overlap && temp[1] != refMax) temp[1] = temp[0];
        refMin = sq::maths::clamp(temp[0], min, max);
        refMax = sq::maths::clamp(temp[1], min, max);
        return true;
    }
    return false;
}

} // anonymous namespace

//============================================================================//

bool ImGui::DragScalarRange2(const char* label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed, const void* p_min, const void* p_max, const char* format, float power)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    const auto ranges = impl_scalar_range_2_get_ranges(data_type, p_current_min, p_current_max, p_min, p_max);

    bool value_changed = false;
    value_changed |= DragScalar("##min", data_type, p_current_min, v_speed, &ranges[0], &ranges[1], format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragScalar("##max", data_type, p_current_max, v_speed, &ranges[2], &ranges[3], format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

//----------------------------------------------------------------------------//

bool ImPlus::InputString(CStrView label, std::string& str, ImGuiInputTextFlags flags,
                         ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputText(label, str.data(), str.capacity() + 1, flags, impl_input_text_callback, &cbUserData);
}

bool ImPlus::InputStringMultiline(CStrView label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags,
                                  ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return ImGui::InputTextMultiline(label, str.data(), str.capacity() + 1, size, flags, impl_input_text_callback, &cbUserData);
}

//----------------------------------------------------------------------------//

ImPlus::DialogResult ImPlus::DialogConfirmation(CStrView title, CStrView message)
{
    const ImGuiID id = GImGui->CurrentWindow->GetID(title);
    if (!ImGui::IsPopupOpen(id, ImGuiPopupFlags_None)) ImGui::OpenPopupEx(id);

    DialogResult result = DialogResult::None;

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (message != nullptr) ImGui::TextUnformatted(message);

        if (ImGui::Button("Confirm") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
        {
            result = DialogResult::Confirm;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
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

//----------------------------------------------------------------------------//

#define IMPLUS_INPUT_VALUE_DEFINITION(Type) \
bool ImPlus::InputValue(CStrView label, Type& ref, Type step, const char* format) \
{ return impl_InputValue(label, ref, step, format); }

#define IMPLUS_DRAG_VALUE_DEFINITION(Type) \
bool ImPlus::DragValue(CStrView label, Type& ref, Type min, Type max, float speed, const char* format) \
{ return impl_DragValue(label, ref, min, max, speed, format); }

#define IMPLUS_SLIDER_VALUE_DEFINITION(Type) \
bool ImPlus::SliderValue(CStrView label, Type& ref, Type min, Type max, const char* format) \
{ return impl_SliderValue(label, ref, min, max, format); }

#define IMPLUS_INPUT_VALUE_RANGE2_DEFINITION(Type) \
bool ImPlus::InputValueRange2(CStrView label, Type& refMin, Type& refMax, Type step, const char* format) \
{ return impl_InputValueRange2(label, refMin, refMax, step, format); }

#define IMPLUS_DRAG_VALUE_RANGE2_DEFINITION(Type) \
bool ImPlus::DragValueRange2(CStrView label, Type& refMin, Type& refMax, Type min, Type max, float speed, const char* format) \
{ return impl_DragValueRange2(label, refMin, refMax, min, max, speed, format); }

#define IMPLUS_SLIDER_VALUE_RANGE2_DEFINITION(Type) \
bool ImPlus::SliderValueRange2(CStrView label, Type& refMin, Type& refMax, Type min, Type max, const char* format) \
{ return impl_SliderValueRange2(label, refMin, refMax, min, max, format); }

//----------------------------------------------------------------------------//

#define IMPLUS_INPUT_FUNCTION_DEFINITIONS(Type) \
IMPLUS_INPUT_VALUE_DEFINITION(Type) \
IMPLUS_DRAG_VALUE_DEFINITION(Type) \
IMPLUS_SLIDER_VALUE_DEFINITION(Type) \
IMPLUS_INPUT_VALUE_RANGE2_DEFINITION(Type) \
IMPLUS_DRAG_VALUE_RANGE2_DEFINITION(Type) \
IMPLUS_SLIDER_VALUE_RANGE2_DEFINITION(Type)

IMPLUS_INPUT_FUNCTION_DEFINITIONS(int8_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(uint8_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(int16_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(uint16_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(int32_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(uint32_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(int64_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(uint64_t)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(float)
IMPLUS_INPUT_FUNCTION_DEFINITIONS(double)
