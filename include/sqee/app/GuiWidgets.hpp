// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/Types.hpp>

#include <dearimgui/imconfig.h> // IWYU pragma: export
#include <dearimgui/imgui.h> // IWYU pragma: export

//============================================================================//

namespace ImGui {

// Like DragIntRange2 or DragFloatRange2, but supporting other types.
// todo: get this merged into imgui itself
SQEE_API bool DragScalarRange2(const char* label, ImGuiDataType data_type, void* p_current_min, void* p_current_max, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, const char* format_max = NULL, ImGuiSliderFlags flags = 0);

} // namespace ImGui

//============================================================================//

namespace ImPlus {

//----------------------------------------------------------------------------//

/// Allows ImPlus functions to take either std or c strings.
/// Can't use std::string_view everywhere because it doesn't guarentee null termination.
class CStrView final
{
public:
    constexpr CStrView() = delete;
    constexpr CStrView(const CStrView&) = default;

    constexpr CStrView(const char* str) : mCharPtr(str) {}
    CStrView(const std::string& str) : mCharPtr(str.c_str()) {}

    constexpr const char* c_str() const { return mCharPtr; }
    constexpr operator const char*() const { return mCharPtr; }

private:
    const char* const mCharPtr;
};

//----------------------------------------------------------------------------//

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

constexpr const int MOUSE_LEFT  = 0;
constexpr const int MOUSE_RIGHT = 1;

enum class DialogResult { None, Confirm, Cancel };

//============================================================================//

// Here we have c++ friendly overloads for any functions taking c strings as arguments.
// If a function here is not inline, it is because the ImGui func does string formatting, which we don't want.
// Use something like fmt::literals::_format to format your strings.
//
// Any functions that I don't currently use are commented out until I need them,
// and begin* functions are also not here, as they have special wrappers.
// This stuff was originally generated with a python script.


// ID stack/scopes
inline void PushID(std::string_view id) { ImGui::PushID(id.data(), id.data() + id.length()); }
inline ImGuiID GetID(std::string_view id) { return ImGui::GetID(id.data(), id.data() + id.length()); }

// Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
//inline void SetWindowPos(CStrView name, ImVec2 pos, ImGuiCond cond = 0) { ImGui::SetWindowPos(name, pos, cond); }
//inline void SetWindowSize(CStrView name, ImVec2 size, ImGuiCond cond = 0) { ImGui::SetWindowSize(name, size, cond); }
//inline void SetWindowCollapsed(CStrView name, bool collapsed, ImGuiCond cond = 0) { ImGui::SetWindowCollapsed(name, collapsed, cond); }
//inline void SetWindowFocus(CStrView name) { ImGui::SetWindowFocus(name); }

// Widgets: Text
inline void Text(std::string_view text) { ImGui::TextUnformatted(text.data(), text.data() + text.length()); }
SQEE_API void TextColored(ImVec4 col, std::string_view text);
SQEE_API void TextDisabled(std::string_view text);
SQEE_API void TextWrapped(std::string_view text);
SQEE_API void LabelText(CStrView label, std::string_view text);
SQEE_API void BulletText(std::string_view text);

// Widgets: Main
inline bool Button(CStrView label, ImVec2 size = {0,0}) { return ImGui::Button(label, size); }
inline bool SmallButton(CStrView label) { return ImGui::SmallButton(label); }
inline bool InvisibleButton(CStrView str_id, ImVec2 size) { return ImGui::InvisibleButton(str_id, size); }
inline bool ArrowButton(CStrView str_id, ImGuiDir dir) { return ImGui::ArrowButton(str_id, dir); }
inline bool Checkbox(CStrView label, bool* v) { return ImGui::Checkbox(label, v); }
//inline bool CheckboxFlags(CStrView label, unsigned int* flags, unsigned int flags_value) { return ImGui::CheckboxFlags(label, flags, flags_value); }
inline bool RadioButton(CStrView label, bool active) { return ImGui::RadioButton(label, active); }
//inline bool RadioButton(CStrView label, int* v, int v_button) { return ImGui::RadioButton(label, v, v_button); }
inline void ProgressBar(float fraction, ImVec2 size_arg = {-1,0}, CStrView overlay = nullptr) { ImGui::ProgressBar(fraction, size_arg, overlay); }

// Widgets: Combo Box
//inline bool Combo(CStrView label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1) { return ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items); }
//inline bool Combo(CStrView label, int* current_item, CStrView items_separated_by_zeros, int popup_max_height_in_items = -1) { return ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items); }
//inline bool Combo(CStrView label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1) { return ImGui::Combo(label, current_item, items_getter, data, items_count, popup_max_height_in_items); }

// Widgets: Color Editor/Picker
//inline bool ColorEdit3(CStrView label, float col[3], ImGuiColorEditFlags flags = 0) { return ImGui::ColorEdit3(label, col, flags); }
//inline bool ColorEdit4(CStrView label, float col[4], ImGuiColorEditFlags flags = 0) { return ImGui::ColorEdit4(label, col, flags); }
//inline bool ColorPicker3(CStrView label, float col[3], ImGuiColorEditFlags flags = 0) { return ImGui::ColorPicker3(label, col, flags); }
//inline bool ColorPicker4(CStrView label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = nullptr) { return ImGui::ColorPicker4(label, col, flags, ref_col); }
//inline bool ColorButton(CStrView desc_id, ImVec4 col, ImGuiColorEditFlags flags = 0, ImVec2 size = {0,0}) { return ImGui::ColorButton(desc_id, col, flags, size); }

// Widgets: Trees
inline bool TreeNode(CStrView label) { return ImGui::TreeNode(label); }
//SQEE_API bool TreeNode(CStrView str_id, CStrView text);
//SQEE_API bool TreeNode(const void* ptr_id, CStrView text);
//inline bool TreeNodeEx(CStrView label, ImGuiTreeNodeFlags flags = 0) { return ImGui::TreeNodeEx(label, flags); }
//SQEE_API bool TreeNodeEx(CStrView str_id, ImGuiTreeNodeFlags flags, CStrView text);
//SQEE_API bool TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, CStrView text);
//inline void TreePush(CStrView str_id) { ImGui::TreePush(str_id); }
inline bool CollapsingHeader(CStrView label, ImGuiTreeNodeFlags flags = 0) { return ImGui::CollapsingHeader(label, flags); }
//inline bool CollapsingHeader(CStrView label, bool* p_open, ImGuiTreeNodeFlags flags = 0) { return ImGui::CollapsingHeader(label, p_open, flags); }

// Widgets: Selectables
inline bool Selectable(CStrView label, bool selected = false, ImGuiSelectableFlags flags = 0, ImVec2 size = {0,0}) { return ImGui::Selectable(label, selected, flags, size); }
//inline bool Selectable(CStrView label, bool* p_selected, ImGuiSelectableFlags flags = 0, ImVec2 size = {0,0}) { return ImGui::Selectable(label, p_selected, flags, size); }

// Widgets: List Boxes
//inline bool ListBox(CStrView label, int* current_item, const char* const items[], int items_count, int height_in_items = -1) { return ImGui::ListBox(label, current_item, items, items_count, height_in_items); }
//inline bool ListBox(CStrView label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) { return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items); }

// Widgets: Data Plotting
//inline void PlotLines(CStrView label, const float* values, int values_count, int values_offset = 0, CStrView overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = {0,0}, int stride = sizeof(float)) { ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride); }
//inline void PlotLines(CStrView label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, CStrView overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = {0,0}) { ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size); }
//inline void PlotHistogram(CStrView label, const float* values, int values_count, int values_offset = 0, CStrView overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = {0,0}, int stride = sizeof(float)) { ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride); }
//inline void PlotHistogram(CStrView label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, CStrView overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = {0,0}) { ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size); }

// Widgets: Menus
inline bool MenuItem(CStrView label, CStrView shortcut = nullptr, bool selected = false, bool enabled = true) { return ImGui::MenuItem(label, shortcut, selected, enabled); }
//inline bool MenuItem(CStrView label, CStrView shortcut, bool* p_selected, bool enabled = true) { return ImGui::MenuItem(label, shortcut, p_selected, enabled); }

// Tooltips
SQEE_API void SetTooltip(std::string_view text);

// Popups, Modals
inline void OpenPopup(CStrView str_id) { ImGui::OpenPopup(str_id); }
//inline bool OpenPopupOnItemClick(CStrView str_id = nullptr, ImGuiMouseButton mouse_button = 1) { return ImGui::OpenPopupOnItemClick(str_id, 1); }
inline bool IsPopupOpen(CStrView str_id) { return ImGui::IsPopupOpen(str_id); }

// Columns
inline void Columns(int count = 1, CStrView id = nullptr, bool border = true) { ImGui::Columns(count, id, border); }

// Tab Bars, Tabs
inline void SetTabItemClosed(CStrView tab_or_docked_window_label) { ImGui::SetTabItemClosed(tab_or_docked_window_label); }

// Logging/Capture
//inline void LogToFile(int auto_open_depth = -1, CStrView filename = nullptr) { ImGui::LogToFile(auto_open_depth, filename); }
//SQEE_API void LogText(CStrView text);

// Drag and Drop
inline bool SetDragDropPayload(CStrView type, const void* data, size_t sz, ImGuiCond cond = 0) { return ImGui::SetDragDropPayload(type, data, sz, cond); }
inline const ImGuiPayload* AcceptDragDropPayload(CStrView type, ImGuiDragDropFlags flags = 0) { return ImGui::AcceptDragDropPayload(type, flags); }

// Miscellaneous Utilities
//inline CStrView GetStyleColorName(ImGuiCol idx) { return ImGui::GetStyleColorName(idx); }
inline ImVec2 CalcTextSize(std::string_view text, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) { return ImGui::CalcTextSize(text.data(), text.data() + text.length(), hide_text_after_double_hash, wrap_width); }

// Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
//inline CStrView GetClipboardText() { return ImGui::GetClipboardText(); }
//inline void SetClipboardText(CStrView text) { ImGui::SetClipboardText(text); }

// Settings/.Ini Utilities
//inline void LoadIniSettingsFromDisk(CStrView ini_filename) { ImGui::LoadIniSettingsFromDisk(ini_filename); }
//inline void LoadIniSettingsFromMemory(CStrView ini_data, size_t ini_size = 0) { ImGui::LoadIniSettingsFromMemory(ini_data, ini_size); }
//inline void SaveIniSettingsToDisk(CStrView ini_filename) { ImGui::SaveIniSettingsToDisk(ini_filename); }
//inline CStrView SaveIniSettingsToMemory(size_t* out_ini_size = nullptr) { return ImGui::SaveIniSettingsToMemory(out_ini_size); }

//----------------------------------------------------------------------------//

inline float FromScreenRight(float offset)
{
    return ImGui::GetIO().DisplaySize.x - offset;
}

inline float FromScreenBottom(float offset)
{
    return ImGui::GetIO().DisplaySize.y - offset;
}

/// Display a tooltip if the mouse is over the last item.
inline void HoverTooltip(std::string_view text)
{
    if (!ImGui::IsItemHovered()) return;
    ImPlus::SetTooltip(text);
}

/// Display a tooltip at a fixed location, below the item.
inline void HoverTooltipFixed(std::string_view text)
{
    if (!ImGui::IsItemHovered()) return;
    ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
    ImPlus::SetTooltip(text);
}

/// Version of PushFont taking the index of a font.
inline void PushFont(int fontIndex)
{
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[fontIndex]);
}

//----------------------------------------------------------------------------//

/// InputText operating on an auto-resizing std::string.
SQEE_API bool InputString(CStrView label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// InputTextMultiline operating on an auto-resizing std::string.
SQEE_API bool InputStringMultiline(CStrView label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

/// ColorEdit3 that behaves like other ImPlus input widgets.
SQEE_API bool InputColour(CStrView label, sq::Vec3F& colour, ImGuiColorEditFlags flags = 0);

/// ColorEdit4 that behaves like other ImPlus input widgets.
SQEE_API bool InputColour(CStrView label, sq::Vec4F& colour, ImGuiColorEditFlags flags = 0);

/// Show a dialog prompting the user for confirmation.
SQEE_API DialogResult DialogConfirmation(CStrView title, CStrView message = nullptr);

/// Show a little button with a cross on it. TODO: actually show the label
SQEE_API bool CloseButton(CStrView label);

//----------------------------------------------------------------------------//

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

SQEE_API bool is_temp_input_open();

} // namespace detail

//----------------------------------------------------------------------------//

// note: all of the Input* widgets currently suffer from https://github.com/ocornut/imgui/issues/4714
// not worrying about it for now, will hopefully be fixed soon

template <class Type>
inline bool InputValue(CStrView label, Type& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::InputScalar(label, dataType, &temp, step > 0 ? &step : nullptr, nullptr, format);
    if (changed) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool DragValue(CStrView label, Type& ref, float speed, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::DragScalar(label, dataType, &temp, speed, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool SliderValue(CStrView label, Type& ref, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp = ref;
    bool changed = ImGui::SliderScalar(label, dataType, &temp, &min, &max, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <class Type>
inline bool DragValueRange2(CStrView label, Type& refMin, Type& refMax, float speed, decltype(Type()) min, decltype(Type()) max, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    Type temp[2] = { refMin, refMax };
    bool changed = ImGui::DragScalarRange2(label, dataType, &temp[0], &temp[1], speed, &min, &max, format, format, ImGuiSliderFlags_AlwaysClamp);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) { refMin = temp[0]; refMax = temp[1]; }
    return changed;
}

//----------------------------------------------------------------------------//

template <int Size, class Type>
inline bool InputVector(CStrView label, sq::maths::Vector<Size, Type>& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    bool changed = ImGui::InputScalarN(label, dataType, temp.data, Size, step > 0 ? &step : nullptr, nullptr, format);
    if (changed) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

template <int Size, class Type>
inline bool DragVector(CStrView label, sq::maths::Vector<Size, Type>& ref, float speed, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    bool changed = ImGui::DragScalarN(label, dataType, temp.data, Size, speed, nullptr, nullptr, format);
    if (changed && detail::is_temp_input_open()) changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

// todo: make widget fancy, allow to switch input mode to euler XYZ
template <class Type>
inline bool InputQuaternion(CStrView label, sq::maths::Quaternion<Type>& ref, decltype(Type()) step, const char* format = nullptr)
{
    constexpr auto dataType = detail::get_data_type<Type>();
    auto temp = ref;
    ImGui::InputScalarN(label, dataType, temp.data, 4, step > 0 ? &step : nullptr, nullptr, format);
    const bool changed = ImGui::IsItemDeactivatedAfterEdit();
    if (changed) ref = temp;
    return changed;
}

//----------------------------------------------------------------------------//

/// C++ style API for combo boxes. If none is given, it will be listed as index -1.
/// sq::to_c_string(Container::value_type) must be defined.
template <class Container, class Index>
inline bool Combo(CStrView label, const Container& container, Index& ref, CStrView none = nullptr, ImGuiComboFlags flags = 0)
{
    // todo: unsigned types would be fine if none is not given
    static_assert (std::is_signed_v<Index>, "index type must be signed");

    const Index oldValue = ref;

    const auto currentItem = ref == -1 ? none.c_str() : sq::to_c_string(*std::next(container.begin(), ref));
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

    // only return true if selection has changed, unlike ImGui::Combo
    return ref != oldValue;
}

/// Combo box for an enum value. For use with the SQEE_ENUM_HELPER macro.
template <class EnumType>
inline bool ComboEnum(CStrView label, EnumType& ref, ImGuiComboFlags flags = 0)
{
    const EnumType oldValue = ref;

    if (ImGui::BeginCombo(label, sq::enum_to_string(ref), flags))
    {
        for (EnumType e : sq::enum_items_v<EnumType>)
            if (ImGui::Selectable(sq::enum_to_string(e)))
                ref = e;

        ImGui::EndCombo();
    }

    // only return true if selection has changed, unlike ImGui::Combo
    return ref != oldValue;
}

//----------------------------------------------------------------------------//

/// Assign ref to value when pressed.
template <class Type>
inline bool RadioButton(CStrView label, Type& ref, Type value)
{
    const bool pressed = ImGui::RadioButton(label, ref == value);
    if (pressed == true) ref = value;

    return pressed;
}

//----------------------------------------------------------------------------//

/// Wrapper for Begin, End.
struct ScopeWindow final : sq::NonCopyable
{
    bool show;
    ScopeWindow(CStrView name, ImGuiWindowFlags flags) : show(ImGui::Begin(name, nullptr, flags)) {}
    ~ScopeWindow() { ImGui::End(); }
};

/// Wrapper for Begin, End with a close button.
struct ScopeWindowClosable final : sq::NonCopyable
{
    bool open = true; bool show;
    ScopeWindowClosable(CStrView name, ImGuiWindowFlags flags) : show(ImGui::Begin(name, &open, flags)) {}
    ~ScopeWindowClosable() { ImGui::End(); }
};

//----------------------------------------------------------------------------//

/// Wrapper for BeginChild, EndChild.
template <class Body> inline
void if_Child(CStrView id, ImGuiWindowFlags flags, Body body)
{
    if (ImGui::BeginChild(id, ImVec2(), false, flags)) body();
    ImGui::EndChild();
}

/// Wrapper for BeginPopup, EndPopup.
template <class Body> inline
void if_Popup(CStrView id, ImGuiWindowFlags flags, Body body)
{
    if (ImGui::BeginPopup(id, flags)) { body(); ImGui::EndPopup(); }
}

/// Wrapper for BeginPopupContextItem, EndPopup.
template <class Body> inline
void if_PopupContextItem(CStrView id, ImGuiPopupFlags flags, Body body)
{
    if (ImGui::BeginPopupContextItem(id, flags)) { body(); ImGui::EndPopup(); }
}

/// Wrapper for BeginTabBar, EndTabBar.
template <class Body> inline
void if_TabBar(CStrView id, ImGuiTabBarFlags flags, Body body)
{
    if (ImGui::BeginTabBar(id, flags)) { body(); ImGui::EndTabBar(); }
}

/// Wrapper for BeginTabItem, EndTabItem.
template <class Body> inline
void if_TabItem(CStrView label, ImGuiTabItemFlags flags, Body body)
{
    if (ImGui::BeginTabItem(label, nullptr, flags)) { body(); ImGui::EndTabItem(); }
}

/// Wrapper for BeginMenuBar, EndMenuBar.
template <class Body> inline
void if_MenuBar(Body body)
{
    if (ImGui::BeginMenuBar()) { body(); ImGui::EndMenuBar(); }
}

/// Wrapper for BeginMenu, EndMenu.
template <class Body> inline
void if_Menu(CStrView label, bool enabled, Body body)
{
    if (ImGui::BeginMenu(label, enabled)) { body(); ImGui::EndMenu(); }
}

//----------------------------------------------------------------------------//

/// Variant of TabItem that puts contents in a child window.
template <class Body> inline
void if_TabItemChild(CStrView label, ImGuiTabItemFlags flags, Body body)
{
    if (ImGui::BeginTabItem(label, nullptr, flags))
    {
        if (ImGui::BeginChild("", {0,0}, false, 0)) body();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

//----------------------------------------------------------------------------//

/// Wrapper for PushID, PopID.
struct ScopeID final : sq::NonCopyable
{
    ScopeID(const char* cstr_id) { ImGui::PushID(cstr_id); }
    ScopeID(std::string_view sv_id) { ImGui::PushID(sv_id.data(), sv_id.data() + sv_id.length()); }
    ScopeID(const void* ptr_id) { ImGui::PushID(ptr_id); }
    ScopeID(int int_id) { ImGui::PushID(int_id); }
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
    ScopeIndent(float width = 0.f) : width(width) { ImGui::Indent(width); }
    ~ScopeIndent() { ImGui::Unindent(width); }
    const float width;
};

/// Wrapper for Unindent, Indent.
struct ScopeUnindent final : sq::NonCopyable
{
    ScopeUnindent(float width = 0.f) : width(width) { ImGui::Unindent(width); }
    ~ScopeUnindent() { ImGui::Indent(width); }
    const float width;
};

/// Wrapper for PushFont, PopFont.
struct ScopeFont final : sq::NonCopyable
{
    ScopeFont(ImFont* font) { ImGui::PushFont(font); }
    ScopeFont(int fontIndex) { ImPlus::PushFont(fontIndex); }
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

//----------------------------------------------------------------------------//

using Style_ButtonTextAlign = ScopeStyleVec<ImGuiStyleVar_ButtonTextAlign>;
using Style_SelectableTextAlign = ScopeStyleVec<ImGuiStyleVar_SelectableTextAlign>;
using Style_FramePadding = ScopeStyleVec<ImGuiStyleVar_FramePadding>;

//============================================================================//

} // namespace ImPlus
