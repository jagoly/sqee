#pragma once

#include <limits>

#include <functional>

#include <sqee/helpers.hpp>

#include <sqee/builtins.hpp>
#include <sqee/misc/FlagSet.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/redist/imgui/imgui.hpp>

//============================================================================//

namespace sq::gui {

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

enum class Collapse
{
    Close   = 1 << 0, ///< show a close button on the header
    NoFrame = 1 << 1, ///< don't draw a frame around content
};

enum class Window
{
    NoTitle  = 1 << 0, ///< don't display the title bar
    ScrollX  = 1 << 1, ///< always show horizontal scrollbar
    ScrollY  = 1 << 2, ///< always show vertical scrollbar
    MenuBar  = 1 << 3, ///< reserve space for a menu bar widget
    Close    = 1 << 4, ///< show a close button on the title
    NoMove   = 1 << 5, ///< don't allow the user to move the window
    AutoSize = 1 << 6, ///< adjust window size to fit contents
};

enum class InputText
{
    LiveEdit       = 1 << 0, ///< immediately write back to string
    CallbackAlways = 1 << 1, ///< activate callback every frame
    CallbackFilter = 1 << 2, ///< activate callback for text input
    DisableTab     = 1 << 3, ///< prevent tabbing out of the widget
    //AutoHeight     = 1 << 4, ///< adjust height to fit contents // always on
};

struct ScopeResult
{
    bool display;
    bool close;
};

struct InputTextCallbackData
{
    string text;        ///< check or modify the input string
    uint cursorPos;     ///< check or modify text cursor position
    uint selectStart;   ///< check or modify start of selection range
    uint selectEnd;     ///< check or modify end of selection range
    uint32_t character; ///< will be set to zero if not a filter event
};

using InputTextCallback = std::function<void(InputTextCallbackData&)>;

} // namespace sq::gui

SQEE_ALLOW_FLAGS_FOR_ENUM(sq::gui::Collapse);
SQEE_ALLOW_FLAGS_FOR_ENUM(sq::gui::Window);
SQEE_ALLOW_FLAGS_FOR_ENUM(sq::gui::InputText);

//============================================================================//

namespace sq::gui::flags {

using sq::gui::Collapse;
using sq::gui::Window;
using sq::gui::InputText;

} // namespace sq::gui::flags

//============================================================================//

namespace sq::gui {

inline ImGuiIO& get_io() { return imgui::GetIO(); }

inline ImVector<ImFont*>& get_fonts() { return get_io().Fonts->Fonts; }

inline ImGuiStyle get_style() { return imgui::GetStyle(); }

} // namespace sq::gui

//============================================================================//

namespace sq::gui::detail {

struct base_scope_bool : private NonCopyable
{
    operator bool() const { return mResult; }
    protected: bool mResult {};
};

struct base_scope_result : private NonCopyable
{
    operator bool() const { return mResult.display; }
    bool want_display() const { return mResult.display; }
    bool want_close() const { return mResult.close; }
    protected: ScopeResult mResult {};
};

ScopeResult begin_collapse(const char* label, FlagSet<Collapse> flags);

ScopeResult begin_window(const char* label, Vec2F size, Vec2F position, FlagSet<Window> flags);

void begin_framed_group(bool lb, bool lt, bool rb, bool rt);

void end_framed_group();

void display_input_label(const char* label, float width);

bool display_input_text(const char* label, uint maxChars, string& ref, FlagSet<InputText> flags, InputTextCallback callback);

} // namespace sq::gui::detail

//============================================================================//

namespace sq::gui {

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

/// Draw a decorative frame around a group of widgets.
struct scope_framed_group final
{
    scope_framed_group() { detail::begin_framed_group(true, true, true, true); }
    ~scope_framed_group() { detail::end_framed_group(); }
};

//============================================================================//

struct scope_menu_bar final : detail::base_scope_bool
{
    scope_menu_bar() { mResult = imgui::BeginMenuBar(); }
    ~scope_menu_bar() { if (mResult) imgui::EndMenuBar(); }
};

struct scope_menu final : detail::base_scope_bool
{
    scope_menu(string label) { mResult = imgui::BeginMenu(label.c_str()); }
    ~scope_menu() { if (mResult) imgui::EndMenu(); }
};

inline bool menu_item(string label)
{
    return imgui::MenuItem(label.c_str());
}

//============================================================================//

/// Group with a collapsable header that respects PushItemWidth.
///
struct scope_collapse final : detail::base_scope_result
{
    scope_collapse(string label, FlagSet<Collapse> flags, int font = -1)
        : mFlags(flags)
    {
        if (font >= 0) imgui::PushFont(get_fonts()[font]);
        mResult = detail::begin_collapse(label.c_str(), mFlags);
        if (font >= 0) imgui::PopFont();

        if (mResult.display == true)
        {
            if (!(mFlags & Collapse::NoFrame))
            {
                imgui::SetCursorPosY(imgui::GetCursorPosY() - get_style().ItemSpacing.y);
                detail::begin_framed_group(true, false, true, false);
            }
            imgui::PushID(label.c_str());
        }
    }

    //--------------------------------------------------------//

    ~scope_collapse()
    {
        if (mResult.display == true)
        {
            imgui::PopID();

            if (!(mFlags & Collapse::NoFrame))
                detail::end_framed_group();
        }
    }

    //--------------------------------------------------------//

    private: const FlagSet<Collapse> mFlags;
};

//----------------------------------------------------------------------------//

/// An alternative api around imgui for creating windows.
///
struct scope_window final : detail::base_scope_result
{
    scope_window(string name, Vec2F minSize, Vec2F maxSize, Vec2F position, FlagSet<Window> _flags)
        : flags(_flags)
    {
        // setting position relative to inverse sides not supported for moving windows
        //SQASSERT(flags & Window::NoMove || (position.x >= 0.f && position.y >= 0.f), "");
        //SQASSERT(flags & Window::NoMove || (maxSize.x >= 0.f && maxSize.y >= 0.f), "");

        if (!(flags & Window::NoMove)) imgui::SetNextWindowPosCenter();

        const Vec2F displaySize = get_io().DisplaySize;

        if (std::signbit(minSize.x)) minSize.x += displaySize.x;
        if (std::signbit(minSize.y)) minSize.y += displaySize.y;

        if (maxSize.x <= 0.f) maxSize.x += displaySize.x;
        if (maxSize.y <= 0.f) maxSize.y += displaySize.y;

        imgui::SetNextWindowSizeConstraints(minSize, maxSize);

        ImGuiWindowFlags imFlags = ImGuiWindowFlags_NoSavedSettings;

        if (flags & Window::NoMove) imFlags |= ImGuiWindowFlags_NoMove;
        if (flags & Window::AutoSize) imFlags |= ImGuiWindowFlags_AlwaysAutoResize;

        if (flags & Window::NoTitle) imFlags |= ImGuiWindowFlags_NoTitleBar;
        if (flags & Window::MenuBar) imFlags |= ImGuiWindowFlags_MenuBar;

        if (flags & Window::ScrollX) imFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (flags & Window::ScrollY) imFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

        if (flags & Window::Close)
        {
            bool stayOpen = true;
            mResult.display = imgui::Begin(name.c_str(), &stayOpen, imFlags);
            mResult.close = !stayOpen;
        }
        else mResult.display = imgui::Begin(name.c_str(), nullptr, imFlags);

        // these would work, but would cause an ugly delay when the window resizes
        //SQASSERT(!(std::signbit(position.x) && (flags & Window::ShrinkX)), "");
        //SQASSERT(!(std::signbit(position.y) && (flags & Window::ShrinkY)), "");

        const Vec2F windowSize = imgui::GetWindowSize();

        if (std::signbit(position.x)) position.x += displaySize.x - windowSize.x;
        if (std::signbit(position.y)) position.y += displaySize.y - windowSize.y;

        if (flags & Window::NoMove) imgui::SetWindowPos(position);
    }

    //--------------------------------------------------------//

    ~scope_window() { imgui::End(); }

    //--------------------------------------------------------//

    private: const FlagSet<Window> flags;
};

//============================================================================//

/// Display a simple line of text.
///
/// @param text the string to display
///
inline void display_text(string text)
{
    imgui::TextUnformatted(text.c_str());
}

//----------------------------------------------------------------------------//

/// Integer input widget with a label and step buttons.
///
/// @tparam T integer type (int/uint, 8/16/32)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero for no label)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param min,max the range of values to allow
///
/// @result was the value just modified
///
template <class T> inline
bool input_int ( string label, float labelWidth, T& ref, int step,
                 int min = std::max<long>(INT32_MIN, std::numeric_limits<T>::min()),
                 int max = std::min<long>(INT32_MAX, std::numeric_limits<T>::max()) )
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asInt32 = static_cast<int>(ref);
    bool changed = imgui::InputInt(hiddenLabel.c_str(), &asInt32, step, 0);
    ref = static_cast<T>(maths::clamp(asInt32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Float input widget with a label and step buttons.
///
/// @tparam T float type (float, double, long double)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal point
/// @param min,max the range of values to allow
///
/// @result was the value just modified
///
template <class T> inline
bool input_float ( string label, float labelWidth, T& ref, float step, int precision,
                   float min = std::numeric_limits<float>::lowest(),
                   float max = std::numeric_limits<float>::max() )
{
    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asFloat32 = static_cast<float>(ref);
    bool changed = imgui::InputFloat(hiddenLabel.c_str(), &asFloat32, step, 0.f, precision);
    ref = static_cast<T>(maths::clamp(asFloat32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Integer vector input widget with a label.
///
/// @tparam S number of vector components
/// @tparam T integer type (int/uint, 8/16/32/64)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param min,max the range of values to allow
///
/// @result was the value just modified
///
template <int S, class T> inline
bool input_vec_i ( string label, float labelWidth, Vector<S,T>& ref,
                   Vector<S,int> min = Vector<S,int>(std::numeric_limits<T>::min()),
                   Vector<S,int> max = Vector<S,int>(std::numeric_limits<T>::max()) )
{
    // todo: see if compilers optimize this out when min and max are constexpr
    for (int i=0; i<S; ++i) SQASSERT(min[i] >= std::numeric_limits<T>::min(), "");
    for (int i=0; i<S; ++i) SQASSERT(max[i] >= std::numeric_limits<T>::max(), "");

    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asInt32 = Vector<S,int>(ref); bool changed;
    if constexpr (S == 2) changed = imgui::InputInt2(hiddenLabel.c_str(), asInt32.data);
    if constexpr (S == 3) changed = imgui::InputInt3(hiddenLabel.c_str(), asInt32.data);
    if constexpr (S == 4) changed = imgui::InputInt4(hiddenLabel.c_str(), asInt32.data);
    ref = Vector<S,T>(maths::clamp(asInt32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Float vector input widget with a label.
///
/// @tparam S number of vector components
/// @tparam T float type (float, double, long double)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label (zero to hide)
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal points
/// @param min,max the range of values to allow
///
/// @result was the value just modified
///
template <int S, class T> inline
bool input_vec_f ( string label, float labelWidth, Vector<S,T>& ref, int precision,
                   Vector<S,float> min = Vector<S,float>(std::numeric_limits<T>::min()),
                   Vector<S,float> max = Vector<S,float>(std::numeric_limits<T>::max()) )
{
    // todo: see if compilers optimize this out when min and max are constexpr
    for (int i=0; i<S; ++i) SQASSERT(min[i] >= std::numeric_limits<T>::min(), "");
    for (int i=0; i<S; ++i) SQASSERT(max[i] >= std::numeric_limits<T>::max(), "");

    if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asFloat32 = Vector<S,float>(ref); bool changed;
    if constexpr (S == 2) changed = imgui::InputFloat2(hiddenLabel.c_str(), asFloat32.data, precision);
    if constexpr (S == 3) changed = imgui::InputFloat3(hiddenLabel.c_str(), asFloat32.data, precision);
    if constexpr (S == 4) changed = imgui::InputFloat4(hiddenLabel.c_str(), asFloat32.data, precision);
    ref = Vector<S,T>(maths::clamp(asFloat32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Single line text input widget with a label.
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
}

//----------------------------------------------------------------------------//

/// Multi line text input widget.
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
}

//----------------------------------------------------------------------------//

/// Combo Box widget.
///
template <class IntType> inline
bool input_combo(string label, float labelWidth, IntType& ref, const std::vector<string>& items)
{
    detail::display_input_label(label.c_str(), labelWidth);

    const size_t itemCount = items.size();
    SQASSERT(itemCount <= std::numeric_limits<IntType>::max(), "");

    std::vector<const char*> cStrItems;
    cStrItems.reserve(itemCount);

    for (const string& item : items) cStrItems.push_back(item.c_str());

    const string hiddenLabel = "##" + label;

    auto asInt32 = static_cast<int>(ref);
    bool changed = imgui::Combo(hiddenLabel.c_str(), &asInt32, cStrItems.data(), int(itemCount));
    ref = static_cast<IntType>(asInt32);

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
