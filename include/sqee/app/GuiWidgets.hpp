// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>

#include <sqee/redist/imgui/imgui.hpp>

// Some extra imgui widgets, but not overkill like I had before.

namespace ImGui {

//============================================================================//

// Like DragIntRange2 or DragFloatRange2, but supporting other types
SQEE_API bool DragScalarRange2(const char* label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed, const void* p_min, const void* p_max, const char* format = NULL, float power = 1.0f);

//----------------------------------------------------------------------------//

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

constexpr const int MOUSE_LEFT  = 0;
constexpr const int MOUSE_RIGHT = 1;

enum class DialogResult { None, Confirm, Cancel };

//----------------------------------------------------------------------------//

inline void HoverTooltip(const char* text)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(text);
        ImGui::EndTooltip();
    }
}

inline float FromScreenRight(float offset)
{
    return ImGui::GetIO().DisplaySize.x - offset;
}

inline float FromScreenBottom(float offset)
{
    return ImGui::GetIO().DisplaySize.y - offset;
}

inline void Text(const std::string& text)
{
    ImGui::TextUnformatted(text.c_str());
}

//----------------------------------------------------------------------------//

/// InputText operating on an auto-resizing std::string.
SQEE_API bool InputString(const char* label, std::string& str, ImGuiInputTextFlags flags = 0,
                          ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// InputTextMultiline operating on an auto-resizing std::string.
SQEE_API bool InputStringMultiline(const char* label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags = 0,
                                   ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// Show a dialog prompting the user for confirmation.
SQEE_API DialogResult DialogConfirmation(const char* title, const char* message);

/// Show a little button with a cross on it. TODO: actually show the label
SQEE_API bool CloseButton(const char* label);

/// Input for a minimum and maximum hue.
SQEE_API bool InputRangeHue(const char* label, float& min, float& max);

//----------------------------------------------------------------------------//

template <class Type>
constexpr inline ImGuiDataType_ impl_get_data_type()
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

template <class Type>
SQEE_API bool InputValue(const char* label, Type& ref, decltype(Type()) step = 0, const char* format = nullptr);

//template <class Type>
//SQEE_API bool InputValue(const char* label, Type& ref, decltype(Type()) min, decltype(Type()) max, decltype(Type()) step = 0, const char* format = nullptr);

//template <class Type>
//SQEE_API bool DragValue(const char* label, Type& ref, float speed, const char* format = nullptr);

template <class Type>
SQEE_API bool DragValue(const char* label, Type& ref, decltype(Type()) min, decltype(Type()) max, float speed, const char* format = nullptr);

template <class Type>
SQEE_API bool SliderValue(const char* label, Type& ref, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr);

template <class Type>
SQEE_API bool InputValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) step = 0, const char* format = nullptr);

//template <class Type>
//SQEE_API bool InputValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) min, decltype(Type()) max, decltype(Type()) step = 0, const char* format = nullptr);

//template <class Type>
//SQEE_API bool DragValueRange2(const char* label, Type& refMin, Type& refMax, float speed, const char* format = nullptr);

template <class Type>
SQEE_API bool DragValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) min, decltype(Type()) max, float speed, const char* format = nullptr);

template <class Type>
SQEE_API bool SliderValueRange2(const char* label, Type& refMin, Type& refMax, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr);

template <int Size, class Type>
inline bool InputVector(const char* label, sq::maths::Vector<Size, Type>& ref, decltype(Type()) step = 0, const char* format = nullptr)
{
    constexpr const auto dataType = impl_get_data_type<Type>();
    auto temp = ref;
    ImGui::InputScalarN(label, dataType, temp.data, Size, step > 0 ? &step : nullptr, nullptr, format);
    const bool changed = IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

//----------------------------------------------------------------------------//

/// C++ style API for combo boxes. If none is given, it will be listed as index -1.
/// sq::to_c_string(Container::value_type) must be defined.
template <class Container, class Index>
inline bool ComboPlus(const char* label, const Container& container, Index& ref, const char* none = nullptr, ImGuiComboFlags flags = 0)
{
    // todo: unsigned types would be fine if none is not given
    static_assert (std::is_signed_v<Index>, "index type must be signed");

    const Index oldValue = ref;

    const auto currentItem = ref == -1 ? none : sq::to_c_string(*std::next(container.begin(), ref));
    if (ImGui::BeginCombo(label, currentItem, flags))
    {
        if (none != nullptr && ImGui::Selectable(none))
            ref = -1;

        Index index = 0;
        for (auto iter = container.begin(); iter != container.end(); ++iter, ++index)
            if (ImGui::Selectable(sq::to_c_string(*iter)))
                ref = index;

        ImGui::EndCombo();
    }

    // only return true if new selection has changed, unlike ImGui::Combo
    return ref != oldValue;
}

/// Combo box for an enum value. For use with the SQEE_ENUM_HELPER macro.
template <class EnumType>
inline bool ComboEnum(const char* label, EnumType& ref, ImGuiComboFlags flags = 0)
{
    const EnumType oldValue = ref;

    if (ImGui::BeginCombo(label, sq::enum_to_string(ref), flags))
    {
        for (auto i = sq::EnumHelper<EnumType>::begin(); i != sq::EnumHelper<EnumType>::end(); ++i)
            if (ImGui::Selectable(sq::enum_to_string(EnumType(i))))
                ref = EnumType(i);

        ImGui::EndCombo();
    }

    // only return true if new selection is different, unlike ImGui::Combo
    return ref != oldValue;
}

//----------------------------------------------------------------------------//

/// Assign ref to value when pressed.
template <class Type>
inline bool RadioButton(const char* label, Type& ref, Type value)
{
    const bool pressed = ImGui::RadioButton(label, ref == value);
    if (pressed == true) ref = value;

    return pressed;
}

//----------------------------------------------------------------------------//

/// Wrapper for Begin, End.
struct ScopeWindow final : sq::NonCopyable
{
    const bool open;
    ScopeWindow(const char* name, ImGuiWindowFlags flags) : open(ImGui::Begin(name, nullptr, flags)) {}
    ~ScopeWindow() { ImGui::End(); }
};

/// Wrapper for PushID, PopID.
struct ScopeID final : sq::NonCopyable
{
    ScopeID(int int_id) { ImGui::PushID(int_id); }
    ScopeID(const char* str_id) { ImGui::PushID(str_id); }
    ScopeID(const void* ptr_id) { ImGui::PushID(ptr_id); }
    ~ScopeID() { ImGui::PopID(); }
};

/// Wrapper for PushItemWidth, PopItemWidth.
struct ScopeItemWidth final : sq::NonCopyable
{
    ScopeItemWidth(float width) { ImGui::PushItemWidth(width); }
    ~ScopeItemWidth() { ImGui::PopItemWidth(); }
};

/// Wrapper for Indent, Unindent.
struct ScopeIndent final : sq::NonCopyable
{
    ScopeIndent() { ImGui::Indent(); }
    ~ScopeIndent() { ImGui::Unindent(); }
};

/// Wrapper for Unindent, Indent.
struct ScopeUnindent final : sq::NonCopyable
{
    ScopeUnindent() { ImGui::Unindent(); }
    ~ScopeUnindent() { ImGui::Indent(); }
};

/// Wrapper for PushFont, PopFont.
struct ScopeFont final : sq::NonCopyable
{
    ScopeFont(int fontIndex) { ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[fontIndex]); }
    ~ScopeFont() { ImGui::PopFont(); }
};

/// Wrapper for PushStyleVar(float), PopStyleVar.
template <ImGuiStyleVar_ StyleVar>
struct ScopeStyleFloat final : sq::NonCopyable
{
    ScopeStyleFloat(float value) { ImGui::PushStyleVar(StyleVar, value); }
    ~ScopeStyleFloat() { ImGui::PopStyleVar(); }
};

/// Wrapper for PushStyleVar(ImVec2), PopStyleVar.
template <ImGuiStyleVar_ StyleVar>
struct ScopeStyleVec final : sq::NonCopyable
{
    ScopeStyleVec(float x, float y) { ImGui::PushStyleVar(StyleVar, {x, y}); }
    ~ScopeStyleVec() { ImGui::PopStyleVar(); }
};

using ScopeStyle_ButtonTextAlign = ScopeStyleVec<ImGuiStyleVar_ButtonTextAlign>;

//============================================================================//

} // namespace ImGui
