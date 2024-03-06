#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/TypeAliases.hpp>
#include <sqee/core/TypeTraits.hpp>

// defined here instead of in imconfig.h so that we have access to StackString
#define IM_STRV_CLASS_EXTRA \
    ImStrv(const std::string& s) : Begin(s.data()), End(s.data() + s.length()) {} \
    ImStrv(const std::string_view& s) : Begin(s.data()), End(s.data() + s.length()) {} \
    template <size_t Capacity> \
    ImStrv(const sq::StackString<Capacity>& s) : Begin(s.data()), End(s.data() + s.length()) {}

#include <dearimgui/imconfig.h> // IWYU pragma: export
#include <dearimgui/imgui.h> // IWYU pragma: export

#include <cassert>

//==============================================================================

namespace ImGui {

// Like DragIntRange2 or DragFloatRange2, but supporting other types.
// todo: get this merged into imgui itself
SQEE_API bool DragScalarRange2(ImStrv label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, const char* format_max = NULL, ImGuiSliderFlags flags = 0);

} // namespace ImGui

namespace ImPlus { //###########################################################

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

constexpr const int MOUSE_LEFT  = 0;
constexpr const int MOUSE_RIGHT = 1;

enum class DialogResult { None, Confirm, Cancel };

/// Only perform formatting and constexpr checking if arguments are given.
template <class... Args>
using FormatString = std::conditional_t<sizeof...(Args) != 0, fmt::format_string<Args...>, fmt::string_view>;

//------------------------------------------------------------------------------

inline void Text(std::string_view text) { ImGui::TextUnformatted(text); }

SQEE_API void TextColored(const ImVec4& col, std::string_view text);

SQEE_API void TextDisabled(std::string_view text);

SQEE_API void TextWrapped(std::string_view text);

SQEE_API void LabelText(ImStrv label, std::string_view text);

SQEE_API void BulletText(std::string_view text);

SQEE_API void SetTooltip(std::string_view text);

//------------------------------------------------------------------------------

/// DisplaySize.x - offset
SQEE_API float FromScreenRight(float offset);

/// DisplaySize.y - offset
SQEE_API float FromScreenBottom(float offset);

/// Version of PushFont taking the index of a font.
SQEE_API void PushFont(int index);

/// Arrange a set of same-width widgets over one or more lines. Call between widgets, like SameLine().
SQEE_API void AutoArrange(float minItemWidth);

/// InputText operating on an auto-resizing std::string.
SQEE_API bool InputString(ImStrv label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// InputTextMultiline operating on an auto-resizing std::string.
SQEE_API bool InputStringMultiline(ImStrv label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// ColorEdit3 that behaves like other ImPlus input widgets.
SQEE_API bool InputColour(ImStrv label, sq::Vec3F& colour, ImGuiColorEditFlags flags = 0);

/// ColorEdit4 that behaves like other ImPlus input widgets.
SQEE_API bool InputColour(ImStrv label, sq::Vec4F& colour, ImGuiColorEditFlags flags = 0);

/// Show a dialog prompting the user for confirmation.
SQEE_API DialogResult DialogConfirmation(ImStrv title, std::string_view message = {});

//------------------------------------------------------------------------------

SQEE_API void HoverTooltipV(bool delay, ImGuiDir dir, fmt::string_view fstr, fmt::format_args args);

/// Display a formatted tooltip if the mouse is over the last item.
template <class... Args>
inline void HoverTooltip(bool delay, ImGuiDir dir, FormatString<Args...> fstr, Args&&... args) { HoverTooltipV(delay, dir, fstr, fmt::make_format_args(args...)); }

/// Display a formatted tooltip if the mouse is over the last item.
template <class... Args>
inline void HoverTooltip(FormatString<Args...> fstr, Args&&... args) { HoverTooltipV(true, ImGuiDir_None, fstr, fmt::make_format_args(args...)); }

//------------------------------------------------------------------------------

namespace detail {

template <class Type>
constexpr ImGuiDataType_ get_data_type()
{
    if constexpr (std::is_same_v<Type, int8_t>)   return ImGuiDataType_S8;
    if constexpr (std::is_same_v<Type, uint8_t>)  return ImGuiDataType_U8;
    if constexpr (std::is_same_v<Type, int16_t>)  return ImGuiDataType_S16;
    if constexpr (std::is_same_v<Type, uint16_t>) return ImGuiDataType_U16;
    if constexpr (std::is_same_v<Type, int32_t>)  return ImGuiDataType_S32;
    if constexpr (std::is_same_v<Type, uint32_t>) return ImGuiDataType_U32;
    if constexpr (std::is_same_v<Type, int64_t>)  return ImGuiDataType_S64;
    if constexpr (std::is_same_v<Type, uint64_t>) return ImGuiDataType_U64;
    if constexpr (std::is_same_v<Type, float>)    return ImGuiDataType_Float;
    if constexpr (std::is_same_v<Type, double>)   return ImGuiDataType_Double;
}

struct ScopeBase
{
    ScopeBase() = default;
    ScopeBase(const ScopeBase&) = delete;
    ScopeBase& operator=(const ScopeBase&) = delete;
    ScopeBase(ScopeBase&&) = delete;
    ScopeBase& operator=(ScopeBase&&) = delete;
};

SQEE_API bool is_temp_input_open();

} // namespace detail

//------------------------------------------------------------------------------

template <class Type>
inline bool InputValue(ImStrv label, Type& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::InputScalar(label, dataType, &temp, step > 0 ? &step : nullptr, nullptr, format);
    if (changed) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool DragValue(ImStrv label, Type& ref, float speed, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::DragScalar(label, dataType, &temp, speed, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool SliderValue(ImStrv label, Type& ref, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::SliderScalar(label, dataType, &temp, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool DragValueRange2(ImStrv label, Type& refMin, Type& refMax, float speed, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type tempMin = refMin, tempMax = refMax;
    bool changed = ImGui::DragScalarRange2(label, dataType, &tempMin, &tempMax, speed, &min, &max, format, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) { refMin = tempMin; refMax = tempMax; }
    return changed;
}

//------------------------------------------------------------------------------

template <int Size, class Type>
inline bool InputVector(ImStrv label, sq::maths::Vector<Size, Type>& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    bool changed = ImGui::InputScalarN(label, dataType, temp.data(), Size, step > 0 ? &step : nullptr, nullptr, format);
    if (changed) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <int Size, class Type>
inline bool DragVector(ImStrv label, sq::maths::Vector<Size, Type>& ref, float speed, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    bool changed = ImGui::DragScalarN(label, dataType, temp.data(), Size, speed, nullptr, nullptr, format);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

// todo: make widget fancy, allow to switch input mode to euler XYZ
template <class Type>
inline bool InputQuaternion(ImStrv label, sq::maths::Quaternion<Type>& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    bool changed = ImGui::InputScalarN(label, dataType, temp.data(), 4, step > 0 ? &step : nullptr, nullptr, format);
    if (changed) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

//------------------------------------------------------------------------------

/// C++ style API for combo boxes. If none is given, it can be selected to set the index to -1.
template <class Container, class Index>
inline bool ComboIndex(ImStrv label, const Container& container, Index& ref, ImStrv none = {}, ImGuiComboFlags flags = 0)
{
    // todo: unsigned types would be fine if none is not given
    static_assert(std::is_signed_v<Index>, "index type must be signed");
    assert(ref >= -1 && ref < int(container.size()));

    // return true if new selection is different to old selection
    bool changed = false;

    if (ImGui::BeginCombo(label, ref == -1 ? none : ImStrv(container[ref]), flags))
    {
        if (none)
        {
            const bool selected = (ref == -1);
            if (ImGui::Selectable(none, selected) && !selected)
                ref = -1, changed = true;
        }

        for (Index index = 0; index < container.size(); ++index)
        {
            const bool selected = (ref == index);
            if (ImGui::Selectable(container[index], selected) && !selected)
                ref = index, changed = true;
        }

        ImGui::EndCombo();
    }

    return changed;
}

/// C++ style API for combo boxes. If none is given, it can be selected to clear the value.
template <class Container, class String>
inline bool ComboString(ImStrv label, const Container& container, String& ref, ImStrv none = {}, ImGuiComboFlags flags = 0)
{
    // return true if new selection is different to old selection
    bool changed = false;

    if (ImGui::BeginCombo(label, ref, flags))
    {
        if (none)
        {
            const bool selected = ref.empty();
            if (ImGui::Selectable(none, selected) && !selected)
                ref = {}, changed = true;
        }

        for (const auto& entry : container)
        {
            const bool selected = (ref == entry);
            if (ImGui::Selectable(entry, selected) && !selected)
                ref = entry, changed = true;
        }

        ImGui::EndCombo();
    }

    return changed;
}

/// Combo box for an enum value. For use with the SQEE_ENUM_HELPER macro.
template <sq::HasEnumHelper Enum>
inline bool ComboEnum(ImStrv label, Enum& ref, ImGuiComboFlags flags = 0)
{
    // return true if new selection is different to old selection
    bool changed = false;

    if (ImGui::BeginCombo(label, sq::enum_to_string(ref), flags))
    {
        for (const Enum value : sq::enum_values_v<Enum>)
        {
            const bool selected = (ref == value);
            if (ImGui::Selectable(sq::enum_to_string(value), selected) && !selected)
                ref = value, changed = true;
        }

        ImGui::EndCombo();
    }

    return changed;
}

//------------------------------------------------------------------------------

/// Assign ref to value when pressed.
template <class Type>
inline bool RadioButton(ImStrv label, Type& ref, Type value)
{
    const bool pressed = ImGui::RadioButton(label, ref == value);
    if (pressed) ref = value;
    return pressed;
}

//------------------------------------------------------------------------------

/// Wrapper for Begin, End.
struct Scope_Window final : detail::ScopeBase
{
    bool show;
    Scope_Window(ImStrv name, ImGuiWindowFlags flags) : show(ImGui::Begin(name, nullptr, flags)) {}
    ~Scope_Window() { ImGui::End(); }
};

/// Variant of Scope_Window with a close button.
struct Scope_WindowClosable final : detail::ScopeBase
{
    bool open = true; bool show;
    Scope_WindowClosable(ImStrv name, ImGuiWindowFlags flags) : show(ImGui::Begin(name, &open, flags)) {}
    ~Scope_WindowClosable() { ImGui::End(); }
};

//------------------------------------------------------------------------------

/// Wrapper for BeginChild, EndChild.
template <class Body>
inline void if_Child(ImStrv id, ImGuiWindowFlags flags, Body body)
{
    if (ImGui::BeginChild(id, ImVec2(), false, flags)) body();
    ImGui::EndChild();
}

/// Wrapper for BeginPopup, EndPopup.
template <class Body>
inline void if_Popup(ImStrv id, ImGuiWindowFlags flags, Body body)
{
    if (ImGui::BeginPopup(id, flags)) { body(); ImGui::EndPopup(); }
}

/// Wrapper for BeginPopupContextItem, EndPopup.
template <class Body>
inline void if_PopupContextItem(ImStrv id, ImGuiPopupFlags flags, Body body)
{
    if (ImGui::BeginPopupContextItem(id, flags)) { body(); ImGui::EndPopup(); }
}

/// Wrapper for BeginTabBar, EndTabBar.
template <class Body>
inline void if_TabBar(ImStrv id, ImGuiTabBarFlags flags, Body body)
{
    if (ImGui::BeginTabBar(id, flags)) { body(); ImGui::EndTabBar(); }
}

/// Wrapper for BeginTabItem, EndTabItem.
template <class Body>
inline void if_TabItem(ImStrv label, ImGuiTabItemFlags flags, Body body)
{
    if (ImGui::BeginTabItem(label, nullptr, flags)) { body(); ImGui::EndTabItem(); }
}

/// Variant of if_TabItem that puts contents in a child window.
template <class Body>
inline void if_TabItemChild(ImStrv label, ImGuiTabItemFlags flags, Body body)
{
    if (ImGui::BeginTabItem(label, nullptr, flags))
    {
        if (ImGui::BeginChild("", ImVec2(), false, 0)) body();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

/// Wrapper for BeginMenuBar, EndMenuBar.
template <class Body>
inline void if_MenuBar(Body body)
{
    if (ImGui::BeginMenuBar()) { body(); ImGui::EndMenuBar(); }
}

/// Wrapper for BeginMenu, EndMenu.
template <class Body>
inline void if_Menu(ImStrv label, bool enabled, Body body)
{
    if (ImGui::BeginMenu(label, enabled)) { body(); ImGui::EndMenu(); }
}

/// Wrapper for TreeNodeEx, TreePop.
template <class Body>
inline void if_TreeNode(ImStrv label, ImGuiTreeNodeFlags flags, Body body)
{
    if (ImGui::TreeNodeEx(label, flags)) { body(); ImGui::TreePop(); }
}

/// Wrapper for BeginTable, EndTable.
template <class Body>
inline void if_Table(ImStrv id, int columns, ImGuiTableFlags flags, ImVec2 outerSize, float innerWidth, Body body)
{
    if (ImGui::BeginTable(id, columns, flags, outerSize, innerWidth)) { body(); ImGui::EndTable(); }
}

//------------------------------------------------------------------------------

/// Wrapper for PushID, PopID.
struct Scope_ID final : detail::ScopeBase
{
    Scope_ID(const char* str_id) { ImGui::PushID(str_id); }
    Scope_ID(ImStrv str_id) { ImGui::PushID(str_id); }
    Scope_ID(const void* ptr_id) { ImGui::PushID(ptr_id); }
    Scope_ID(int int_id) { ImGui::PushID(int_id); }
    ~Scope_ID() { ImGui::PopID(); }
};

/// Wrapper for PushItemWidth, PopItemWidth.
struct Scope_ItemWidth final : detail::ScopeBase
{
    Scope_ItemWidth(float width) { ImGui::PushItemWidth(width); }
    ~Scope_ItemWidth() { ImGui::PopItemWidth(); }
};

/// Wrapper for Indent, Unindent.
struct Scope_Indent final : detail::ScopeBase
{
    float width;
    Scope_Indent(float width) : width(width) { ImGui::Indent(width); }
    ~Scope_Indent() { ImGui::Unindent(width); }
};

/// Wrapper for Unindent, Indent.
struct Scope_Unindent final : detail::ScopeBase
{
    float width;
    Scope_Unindent(float width) : width(width) { ImGui::Unindent(width); }
    ~Scope_Unindent() { ImGui::Indent(width); }
};

/// Wrapper for PushFont, PopFont.
struct Scope_Font final : detail::ScopeBase
{
    Scope_Font(int index) { ImPlus::PushFont(index); }
    ~Scope_Font() { ImGui::PopFont(); }
};

/// Variant of Scope_Font that also temporarily scales the font.
struct Scope_FontScale final : detail::ScopeBase
{
    ImFont* font; float scale;
    Scope_FontScale(int index, float scale) : font(ImGui::GetIO().Fonts->Fonts[index]), scale(font->Scale) { font->Scale = scale; ImGui::PushFont(font); }
    ~Scope_FontScale() { font->Scale = scale; ImGui::PopFont(); }
};

//------------------------------------------------------------------------------

/// Wrapper for PushStyleVar(float), PopStyleVar.
template <ImGuiStyleVar_ StyleVar>
struct Scope_StyleFloat final : detail::ScopeBase
{
    Scope_StyleFloat(float value) { ImGui::PushStyleVar(StyleVar, value); }
    ~Scope_StyleFloat() { ImGui::PopStyleVar(); }
};

using Style_IndentSpacing = Scope_StyleFloat<ImGuiStyleVar_IndentSpacing>;

/// Wrapper for PushStyleVar(ImVec2), PopStyleVar.
template <ImGuiStyleVar_ StyleVar>
struct Scope_StyleVec final : detail::ScopeBase
{
    Scope_StyleVec(float x, float y) { ImGui::PushStyleVar(StyleVar, {x, y}); }
    ~Scope_StyleVec() { ImGui::PopStyleVar(); }
};

using Style_FramePadding = Scope_StyleVec<ImGuiStyleVar_FramePadding>;
using Style_ItemSpacing = Scope_StyleVec<ImGuiStyleVar_ItemSpacing>;
using Style_CellPadding = Scope_StyleVec<ImGuiStyleVar_CellPadding>;
using Style_ButtonTextAlign = Scope_StyleVec<ImGuiStyleVar_ButtonTextAlign>;
using Style_SelectableTextAlign = Scope_StyleVec<ImGuiStyleVar_SelectableTextAlign>;

} // namespace ImPlus ##########################################################

#define IMPLUS_WITH_CONCAT_INNER(a, b) a##b
#define IMPLUS_WITH_CONCAT(a, b) IMPLUS_WITH_CONCAT_INNER(a, b)

/// Declare a variable with a generated name of the given ImPlus class.
#define IMPLUS_WITH(Class) const ImPlus::Class IMPLUS_WITH_CONCAT(implus_with_, __LINE__)
