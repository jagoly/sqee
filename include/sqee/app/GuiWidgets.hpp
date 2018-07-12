#pragma once

#include <sqee/builtins.hpp>

#include <sqee/redist/imgui/imgui.hpp>

//============================================================================//

namespace sq::gui {

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

/*enum class InputText
{
    LiveEdit       = 1 << 0, ///< immediately write back to string
    CallbackAlways = 1 << 1, ///< activate callback every frame
    CallbackFilter = 1 << 2, ///< activate callback for text input
    DisableTab     = 1 << 3, ///< prevent tabbing out of the widget
    //AutoHeight     = 1 << 4, ///< adjust height to fit contents // always on
};

struct InputTextCallbackData
{
    string text;        ///< check or modify the input string
    uint cursorPos;     ///< check or modify text cursor position
    uint selectStart;   ///< check or modify start of selection range
    uint selectEnd;     ///< check or modify end of selection range
    uint32_t character; ///< will be set to zero if not a filter event
};

using InputTextCallback = std::function<void(InputTextCallbackData&)>;*/

} // namespace sq::gui

//============================================================================//

namespace sq::gui {

inline ImGuiIO& get_io() { return imgui::GetIO(); }

inline ImVector<ImFont*>& get_fonts() { return get_io().Fonts->Fonts; }

inline ImGuiStyle get_style() { return imgui::GetStyle(); }

} // namespace sq::gui

//============================================================================//

namespace sq::gui::detail {

void display_input_label(const char* label, float width);

//bool display_input_text(const char* label, uint maxChars, string& ref, FlagSet<InputText> flags, InputTextCallback callback);

} // namespace sq::gui::detail

//============================================================================//

namespace sq::gui {

inline bool begin_collapse(string label, ImGuiTreeNodeFlags flags = 0)
{
    return imgui::TreeNodeEx(label.c_str(), flags | ImGuiTreeNodeFlags_CollapsingHeader);
}

inline void end_collapse()
{
    imgui::TreePop();
}

inline bool begin_window(string name, Vec2F minSize, Vec2F maxSize, Vec2F position, ImGuiWindowFlags flags = 0)
{
    flags = flags | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;

    const Vec2F displaySize = get_io().DisplaySize;

    if (std::signbit(minSize.x)) minSize.x += displaySize.x;
    if (std::signbit(minSize.y)) minSize.y += displaySize.y;

    if (maxSize.x <= 0.f) maxSize.x += displaySize.x;
    if (maxSize.y <= 0.f) maxSize.y += displaySize.y;

    imgui::SetNextWindowSizeConstraints(minSize, maxSize);

    bool result = imgui::Begin(name.c_str(), nullptr, flags);

    const Vec2F windowSize = imgui::GetWindowSize();

    if (std::signbit(position.x)) position.x += displaySize.x - windowSize.x;
    if (std::signbit(position.y)) position.y += displaySize.y - windowSize.y;

    imgui::SetWindowPos(position);

    if (result == false) imgui::End();

    return result;
}

inline void end_window()
{
    imgui::End();
}

//============================================================================//

/// Wrapper for PushFont, PopFont.
struct scope_font final
{
    scope_font(int fontIndex) { imgui::PushFont(get_fonts()[fontIndex]); }
    ~scope_font() { imgui::PopFont(); }
};

//----------------------------------------------------------------------------//

/// Wrapper for PushItemWidth, PopItemWidth.
struct scope_item_width final
{
    scope_item_width(float width) { imgui::PushItemWidth(width); }
    ~scope_item_width() { imgui::PopItemWidth(); }
};

//----------------------------------------------------------------------------//

/// Wrapper for Indent, Unindent.
struct scope_indent final
{
    scope_indent() { imgui::Indent(); }
    ~scope_indent() { imgui::Unindent(); }
};

//----------------------------------------------------------------------------//

/// Wrapper for Unindent, Indent.
struct scope_unindent final
{
    scope_unindent() { imgui::Unindent(); }
    ~scope_unindent() { imgui::Indent(); }
};

//============================================================================//

/// Display a simple line of text.
inline void display_text(string text)
{
    imgui::TextUnformatted(text.c_str());
}

//----------------------------------------------------------------------------//

/// Integer input widget with a label and step buttons.
///
/// @tparam T integer type (int/uint, 8/16/32) (cast from int32)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero for no label)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
///
/// @result was the value just modified
///
template <class T> inline bool input_int(string label, float labelWidth, T& ref, int step = 1)
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asInt32 = static_cast<int>(ref);
    bool changed = imgui::InputInt(hiddenLabel.c_str(), &asInt32, step, 0);
    ref = static_cast<T>(asInt32);

    return changed;
}

//----------------------------------------------------------------------------//

/// Float input widget with a label and step buttons.
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal point
///
/// @result was the value just modified
///
inline bool input_float(string label, float labelWidth, float& ref, float step, int8_t precision)
{
    SQASSERT(precision > 0 && precision < 10, "invalid precision");

    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;
    const char format[5] = {'%', '.', static_cast<char>('0' + precision), 'f', '\0'};

    return imgui::InputFloat(hiddenLabel.c_str(), &ref, step, 0.f, format);
}

//----------------------------------------------------------------------------//

/// Signed Int vector input widget with a label.
///
/// @tparam S number of vector components
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
///
/// @result was the value just modified
///
template <int S> inline bool input_vec_i(string label, float labelWidth, Vector<S,int>& ref)
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    return imgui::InputScalarN(hiddenLabel.c_str(), ImGuiDataType_S32, ref.data, S, nullptr, nullptr, "%d");
}

//----------------------------------------------------------------------------//

/// Unsigned Int vector input widget with a label.
///
/// @tparam S number of vector components
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
///
/// @result was the value just modified
///
template <int S> inline bool input_vec_u(string label, float labelWidth, Vector<S,uint>& ref)
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    return imgui::InputScalarN(hiddenLabel.c_str(), ImGuiDataType_U32, ref.data, S, nullptr, nullptr, "%u");
}

//----------------------------------------------------------------------------//

/// Float vector input widget with a label.
///
/// @tparam S number of vector components
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
/// @param precision digits to display after the decimal points
///
/// @result was the value just modified
///
template <int S> inline
bool input_vec_f(string label, float labelWidth, Vector<S,float>& ref, int8_t precision)
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;
    const char format[5] = {'%', '.', static_cast<char>('0' + precision), 'f', '\0'};

    return imgui::InputScalarN(hiddenLabel.c_str(), ImGuiDataType_Float, ref.data, S, nullptr, nullptr, format);
}

//----------------------------------------------------------------------------//

/*/// Single line text input widget with a label.
///
/// @tparam MaxChars maxiumum string size to allow
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the string to manipulate
/// @param flags set of TextInput flags to use
/// @param callback function to use for callbacks
///
/// @result was the string just modified
///
template <uint MaxChars = 63u> inline
bool input_text ( string label, float labelWidth, string& ref, FlagSet<InputText> flags, InputTextCallback callback )
{
    constexpr const size_t BufSize = MaxChars + 1u;

    //SQASSERT(ref.size() <= MaxChars, "");
    if (ref.size() > MaxChars) ref.resize(MaxChars);

    SQASSERT(callback == nullptr || flags & (InputText::CallbackAlways | InputText::CallbackFilter), "");

    ImGuiInputTextFlags imFlags = 0;

    if (!(flags & InputText::LiveEdit)) imFlags |= ImGuiInputTextFlags_EnterReturnsTrue;
    if (flags & InputText::CallbackAlways) imFlags |= ImGuiInputTextFlags_CallbackAlways;
    if (flags & InputText::CallbackFilter) imFlags |= ImGuiInputTextFlags_CallbackCharFilter;
    if (flags & InputText::DisableTab) imFlags |= ImGuiInputTextFlags_CallbackCompletion;

    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    std::array<char, BufSize> buffer;
    ref.copy(buffer.data(), MaxChars);
    buffer[ref.size()] = '\0';

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
                SQASSERT(data.text.size() <= MaxChars, "naughty callback!");

                data.text.copy(imData->Buf, MaxChars);
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

        changed = imgui::InputText(hiddenLabel.c_str(), buffer.data(), BufSize, imFlags, cbWrapper, &callback);
    }

    else if (flags & InputText::DisableTab)
    {
        const auto cbDummy = [](ImGuiTextEditCallbackData*) -> int { return 0; };
        changed = imgui::InputText(hiddenLabel.c_str(), buffer.data(), BufSize, imFlags, cbDummy);
    }

    else changed = imgui::InputText(hiddenLabel.c_str(), buffer.data(), BufSize, imFlags);

    //--------------------------------------------------------//

    // need to do our own string compare when EnterReturnsTrue is enabled
    if (changed && (flags & InputText::LiveEdit || ref != string_view(buffer.data())))
    {
        ref = string(buffer.data());
        return true;
    }

    return false;
}*/

//----------------------------------------------------------------------------//

/*/// Multi line text input widget.
///
/// @tparam MaxChars maxiumum string size to allow
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the string to manipulate
/// @param flags set of TextInput flags to use
/// @param callback function to use for callbacks
///
/// @result was the string just modified
///
template <uint MaxChars = 255u> inline
bool input_text_multi ( string label, string& ref, FlagSet<InputText> flags, InputTextCallback callback )
{
    constexpr const size_t BufSize = MaxChars + 1u;

    //SQASSERT(ref.size() <= MaxChars, "");
    if (ref.size() > MaxChars) ref.resize(MaxChars);

    SQASSERT(callback == nullptr || flags & (InputText::CallbackAlways | InputText::CallbackFilter), "");

    ImGuiInputTextFlags imFlags = 0;

    if (!(flags & InputText::LiveEdit)) imFlags |= ImGuiInputTextFlags_EnterReturnsTrue;
    if (flags & InputText::CallbackAlways) imFlags |= ImGuiInputTextFlags_CallbackAlways;
    if (flags & InputText::CallbackFilter) imFlags |= ImGuiInputTextFlags_CallbackCharFilter;
    if (flags & InputText::DisableTab) imFlags |= ImGuiInputTextFlags_CallbackCompletion;

    // using imgui provided label for now
    //if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    std::array<char, BufSize> buffer;
    ref.copy(buffer.data(), MaxChars);
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
                SQASSERT(data.text.size() <= MaxChars, "naughty callback!");

                data.text.copy(imData->Buf, MaxChars);
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

        changed = imgui::InputTextMultiline(label.c_str(), buffer.data(), BufSize, size, imFlags, cbWrapper, &callback);
    }

    else if (flags & InputText::DisableTab)
    {
        const auto cbDummy = [](ImGuiTextEditCallbackData*) -> int { return 0; };
        changed = imgui::InputTextMultiline(label.c_str(), buffer.data(), BufSize, size, imFlags, cbDummy);
    }

    else changed = imgui::InputTextMultiline(label.c_str(), buffer.data(), BufSize, size, imFlags);

    //--------------------------------------------------------//

    // need to do our own string compare when EnterReturnsTrue is enabled
    if (changed && (flags & InputText::LiveEdit || ref != string_view(buffer.data())))
    {
        ref = string(buffer.data());
        return true;
    }

    return false;
}*/

//----------------------------------------------------------------------------//

/// Combo Box widget.
///
template <class IntType, class Getter> inline
bool input_combo(string label, float labelWidth, IntType& ref, Getter getter, IntType count)
{
    detail::display_input_label(label.c_str(), labelWidth);
    const string hiddenLabel = "##" + label;

    //--------------------------------------------------------//

    bool changed = false;

    if (imgui::BeginCombo(hiddenLabel.c_str(), ref > -1 ? getter(ref) : ""))
    {
        for (IntType i = 0; i < count; ++i)
        {
            const bool selected = (i == ref);

            imgui::PushID(i);
            if ((changed = imgui::Selectable(getter(i), selected))) ref = i;
            if (selected == true) imgui::SetItemDefaultFocus();
            imgui::PopID();
        }

        imgui::EndCombo();
    }

    return changed;
}

//----------------------------------------------------------------------------//

/// Simple button with a simple tooltip.
///
/// @param label text to show on the button
/// @param tooltip text to show in the tooltip
///
/// @result was the button just clicked
///
inline bool button_with_tooltip(string label, string tooltip)
{
    const bool clicked = imgui::Button(label.c_str());

    if (imgui::IsItemHovered() == true)
    {
        imgui::BeginTooltip();
        imgui::TextUnformatted(tooltip.c_str());
        imgui::EndTooltip();
    }

    return clicked;
}

//============================================================================//

} // namespace sq::gui
