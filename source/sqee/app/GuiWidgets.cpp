// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <climits>

#include <sqee/app/GuiWidgets.hpp>

#include <sqee/macros.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <sqee/redist/imgui/imgui_internal.hpp>

DISABLE_WARNING_OLD_STYLE_CAST;

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

bool ImGui::InputString(const char* label, std::string& str, ImGuiInputTextFlags flags,
                        ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return InputText(label, str.data(), str.capacity() + 1, flags, impl_input_text_callback, &cbUserData);
}

bool ImGui::InputStringMultiline(const char* label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags,
                                 ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return InputTextMultiline(label, str.data(), str.capacity() + 1, size, flags, impl_input_text_callback, &cbUserData);
}

//----------------------------------------------------------------------------//

ImGui::DialogResult ImGui::DialogConfirmation(const char* title, const char* message)
{
    DialogResult result = DialogResult::None;

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize) == true)
    {
        ImGui::TextUnformatted(message);
        if (ImGui::Button("Confirm")) { result = DialogResult::Confirm; ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) { result = DialogResult::Cancel; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    else result = DialogResult::Cancel;

    return result;
}

//----------------------------------------------------------------------------//

bool ImGui::CloseButton(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems == true)
        return false;

    const ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);

    const float frameHeight = GetFrameHeight();
    const ImRect bbox = { window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + frameHeight, window->DC.CursorPos.y + frameHeight) };

    ItemSize(bbox, 0.0f);
    if (ItemAdd(bbox, id) == false)
        return false;

    bool hovered, held;
    const bool pressed = ButtonBehavior(bbox, id, &hovered, &held);

    const ImU32 frameColour = GetColorU32((held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderFrame(bbox.Min, bbox.Max, frameColour, true, g.Style.FrameRounding);

    const ImVec2 centre = bbox.GetCenter();
    const float extent = (frameHeight - g.Style.FramePadding.y) * 0.25f;
    const ImU32 crossColour = GetColorU32(ImGuiCol_Text);
    window->DrawList->AddLine({ centre.x + extent, centre.y + extent }, { centre.x - extent, centre.y - extent }, crossColour);
    window->DrawList->AddLine({ centre.x + extent, centre.y - extent }, { centre.x - extent, centre.y + extent }, crossColour);

    return pressed;
}

//----------------------------------------------------------------------------//

template <class Type> inline
bool ImGui::InputValue(const char* label, Type& ref, decltype(Type()) step, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    InputScalar(label, dataType, &temp, step > 0 ? &step : nullptr, nullptr, format);
    const bool changed = IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
bool ImGui::DragValue(const char* label, Type& ref, decltype(Type()) min, decltype(Type()) max, float speed, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    const bool tempInputTextActive = GImGui->TempInputTextId != 0 && TempInputTextIsActive(GImGui->CurrentWindow->GetID(label));
    bool changed = DragScalar(label, dataType, &temp, speed, &min, &max, format);
    changed = tempInputTextActive ? IsItemDeactivatedAfterEdit() : changed;
    if (changed) ref = sq::maths::clamp(temp, min, max);
    return changed;
}

template <class Type>
bool ImGui::SliderValue(const char* label, Type& ref, decltype(Type()) min, decltype(Type()) max, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    const bool tempInputTextActive = GImGui->TempInputTextId != 0 && TempInputTextIsActive(GImGui->CurrentWindow->GetID(label));
    bool changed = SliderScalar(label, dataType, &temp, &min, &max, format);
    changed = tempInputTextActive ? IsItemDeactivatedAfterEdit() : changed;
    if (changed) ref = sq::maths::clamp(temp, min, max);
    return changed;
}

template <class Type>
bool ImGui::InputValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) step, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    InputScalarN(label, dataType, temp, 2, step > 0 ? &step : nullptr, nullptr, format);
    if (IsItemDeactivatedAfterEdit())
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
bool ImGui::DragValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) min, decltype(Type()) max, float speed, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    const bool changed = ImGui::DragScalarN(label, dataType, temp, 2, speed, &min, &max, format);
    if (changed && (GImGui->TempInputTextId == 0 || IsItemDeactivatedAfterEdit()))
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
bool ImGui::SliderValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) min, decltype(Type()) max, const char* format)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    const bool changed = ImGui::SliderScalarN(label, dataType, temp, 2, &min, &max, format);
    if (changed && (GImGui->TempInputTextId == 0 || IsItemDeactivatedAfterEdit()))
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

//----------------------------------------------------------------------------//

#define SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(Type) \
template bool ImGui::InputValue(const char* label, Type& ref, Type step = 0, const char* format = nullptr); \
template bool ImGui::DragValue(const char* label, Type& ref, Type min, Type max, float speed, const char* format = nullptr); \
template bool ImGui::SliderValue(const char* label, Type& ref, Type min, Type max, const char* format = nullptr); \
template bool ImGui::InputValueRange2(const char* label, Type& refMin, Type& refMax, Type step = 0, const char* format = nullptr); \
template bool ImGui::DragValueRange2(const char* label, Type& refMin, Type& refMax, Type min, Type max, float speed, const char* format = nullptr); \
template bool ImGui::SliderValueRange2(const char* label, Type& refMin, Type& refMax, Type min, Type max, const char* format = nullptr);

SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(int8_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(uint8_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(int16_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(uint16_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(int32_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(uint32_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(int64_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(uint64_t);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(float);
SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS(double);

#undef SQEE_IMGUI_INPUT_TEMPLATE_DEFINITIONS
