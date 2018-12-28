// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>

#include <sqee/redist/imgui/imgui.hpp>

// Some extra imgui widgets, but not overkill like I had before.

namespace ImGui {

//============================================================================//

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
    return GetIO().DisplaySize.x - offset;
}

inline float FromScreenBottom(float offset)
{
    return GetIO().DisplaySize.y - offset;
}

inline void Text(const std::string& text)
{
    TextUnformatted(text.c_str());
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

//----------------------------------------------------------------------------//

#define DEFINE_IMGUI_INPUT_FUNCS_INT(Type, Suffix, Format) \
inline bool InputValue(const char* label, Type& ref, Type step) { \
    return InputScalar(label, ImGuiDataType_##Suffix, &ref, step > 0 ? &step : nullptr, nullptr, Format); }

#define DEFINE_IMGUI_INPUT_FUNCS_FLOAT(Type, Suffix) \
inline bool InputValue(const char* label, Type& ref, Type step, char precision) { \
    const char format[5] = { '%', '.', precision, 'f', 0 }; \
    return InputScalar(label, ImGuiDataType_##Suffix, &ref, step > 0 ? &step : nullptr, nullptr, format); }

#define DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT(Type) \
inline bool InputValue(const char* label, Type& ref, Type step) { \
    int32_t temp = static_cast<int32_t>(ref); \
    const bool result = InputValue(label, temp, step); \
    ref = static_cast<Type>(temp); \
    return result; }

#define DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(Size, Type, Suffix, Format) \
inline bool InputVector(const char* label, sq::maths::Vector<Size, Type>& ref) { \
    return InputScalarN(label, ImGuiDataType_##Suffix, ref.data, Size, nullptr, nullptr, Format); }

#define DEFINE_IMGUI_INPUT_FUNCS_VEC_FLOAT(Size, Type, Suffix) \
inline bool InputVector(const char* label, sq::maths::Vector<Size, Type>& ref, char precision) { \
    const char format[5] = { '%', '.', precision, 'f', 0 }; \
    return InputScalarN(label, ImGuiDataType_##Suffix, ref.data, Size, nullptr, nullptr, format); }

DEFINE_IMGUI_INPUT_FUNCS_INT(int32_t, S32, "%d")
DEFINE_IMGUI_INPUT_FUNCS_INT(uint32_t, U32, "%u")

DEFINE_IMGUI_INPUT_FUNCS_INT(int64_t, S64, "%lld")
DEFINE_IMGUI_INPUT_FUNCS_INT(uint64_t, U64, "%llu")

DEFINE_IMGUI_INPUT_FUNCS_FLOAT(float, Float)
DEFINE_IMGUI_INPUT_FUNCS_FLOAT(double, Double)

DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT(int8_t)
DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT(uint8_t)

DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT(int16_t)
DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT(uint16_t)

DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(2, int, S32, "%d")
DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(3, int, S32, "%d")
DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(4, int, S32, "%d")

DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(2, uint, U32, "%u")
DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(3, uint, U32, "%u")
DEFINE_IMGUI_INPUT_FUNCS_VEC_INT(4, uint, U32, "%u")

DEFINE_IMGUI_INPUT_FUNCS_VEC_FLOAT(2, float, Float)
DEFINE_IMGUI_INPUT_FUNCS_VEC_FLOAT(3, float, Float)
DEFINE_IMGUI_INPUT_FUNCS_VEC_FLOAT(4, float, Float)

#undef DEFINE_IMGUI_INPUT_FUNCS_INT
#undef DEFINE_IMGUI_INPUT_FUNCS_FLOAT
#undef DEFINE_IMGUI_INPUT_FUNCS_SMALL_INT
#undef DEFINE_IMGUI_INPUT_FUNCS_VEC_INT
#undef DEFINE_IMGUI_INPUT_FUNCS_VEC_FLOAT

//----------------------------------------------------------------------------//

template <class EnumType>
inline bool InputSqeeEnumCombo(const char* label, EnumType& ref, ImGuiComboFlags flags)
{
    bool result = false;

    if (ImGui::BeginCombo(label, sq::enum_to_string(ref), flags))
    {
        for (int i = 0; i < sq::enum_count_v<EnumType>; ++i)
            if (ImGui::Selectable(sq::enum_to_string(EnumType(i))))
                ref = EnumType(i), result = true;

        ImGui::EndCombo();
    }

    return result;
}

template <class Type>
inline bool RadioButton(const char* label, Type& ref, Type value)
{
    const bool pressed = RadioButton(label, ref == value);
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
