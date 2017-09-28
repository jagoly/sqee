#pragma once

#include <functional>
#include <limits>

#include <sqee/builtins.hpp>
#include <sqee/misc/FlagSet.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/redist/imgui/imgui.hpp>

namespace sq::gui {

//============================================================================//

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

//----------------------------------------------------------------------------//

enum class WindowFlag
{
    NoTitleBar    = 1 << 0,
    AlwaysScrollX = 1 << 1,
    AlwaysScrollY = 1 << 2,
    ShrinkToFitX  = 1 << 3,
    ShrinkToFitY  = 1 << 4,
    MenuBar       = 1 << 5,
    CloseButton   = 1 << 6
};

//============================================================================//

template <class... Args>
using Callback = std::function<void(Args...)>;

//----------------------------------------------------------------------------//

inline ImGuiIO& get_io() { return imgui::GetIO(); }

inline ImVector<ImFont*>& get_fonts() { return get_io().Fonts->Fonts; }

inline ImGuiStyle get_style() { return imgui::GetStyle(); }

//============================================================================//

namespace detail {

bool CollapsingHeaderSized(const char* label);

void BeginFramedGroup(bool lb, bool lt, bool rb, bool rt);

void EndFramedGroup();

inline void display_input_label(const char* label, float width)
{
    const float cursorStart = imgui::GetCursorPosX();

    const float textWidth = imgui::CalcTextSize(label).x;
    const float padding = (width - textWidth) / 2.f;

    imgui::AlignFirstTextHeightToWidgets();
    imgui::SetCursorPosX(cursorStart + padding - get_style().ItemSpacing.x);
    imgui::TextUnformatted(label);
    imgui::SameLine(cursorStart + width);
}

} // namespace detail

//============================================================================//

/// Base for scopes that need to "return" a bool.
struct base_scope_result_bool
{
    operator bool() const { return mResult; }
    protected: bool mResult;
};

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
    scope_framed_group() { detail::BeginFramedGroup(true, true, true, true); }
    ~scope_framed_group() { detail::EndFramedGroup(); }
};

//----------------------------------------------------------------------------//

/// A collapsable header that respects PushItemWidth and draws a frame around content.
struct scope_collapse : base_scope_result_bool
{
    scope_collapse(string label, int font = -1)
    {
        if (font >= 0) imgui::PushFont(get_fonts()[font]);
        mResult = detail::CollapsingHeaderSized(label.c_str());
        if (font >= 0) imgui::PopFont();

        if (mResult == true)
        {
            imgui::SetCursorPosY(imgui::GetCursorPosY() - get_style().ItemSpacing.y);
            detail::BeginFramedGroup(true, false, true, false);
            imgui::PushID(label.c_str());
        }
    }

    ~scope_collapse()
    {
        if (mResult == true)
        {
            imgui::PopID();
            detail::EndFramedGroup();
        }
    }
};

//----------------------------------------------------------------------------//

/// A moveable and resizeable window.
struct scope_window final
{
    scope_window(string name, bool autoSize, FlagSet<WindowFlag> flags)
    {
        SQASSERT(flags.none_of(WindowFlag::ShrinkToFitX, WindowFlag::ShrinkToFitY), "");

        ImGuiWindowFlags imFlags = ImGuiWindowFlags_NoSavedSettings;

        if (autoSize == true) imFlags |= ImGuiWindowFlags_AlwaysAutoResize;

        if (flags & WindowFlag::NoTitleBar) imFlags |= ImGuiWindowFlags_NoTitleBar;
        if (flags & WindowFlag::AlwaysScrollX) imFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (flags & WindowFlag::AlwaysScrollY) imFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (flags & WindowFlag::MenuBar) imFlags |= ImGuiWindowFlags_MenuBar;

        if (flags & WindowFlag::CloseButton)
        {
            bool stayOpen = true;
            mWantDisplay = imgui::Begin(name.c_str(), &stayOpen, imFlags);
            mWantClose = !stayOpen;
        }
        else mWantDisplay = imgui::Begin(name.c_str(), nullptr, imFlags);
    }

    ~scope_window() { imgui::End(); }

    //--------------------------------------------------------//

    operator bool() const { return want_display(); }

    bool want_display() const { return mWantDisplay; }
    bool want_close() const { return mWantClose; }

    private: bool mWantDisplay = false;
    private: bool mWantClose = false;
};

//----------------------------------------------------------------------------//

/// A window taking up a fixed region of the screen.
struct scope_window_fixed final
{
    scope_window_fixed(string name, Vec2F size, Vec2F position, FlagSet<WindowFlag> flags)
    {
        const Vec2F displaySize = get_io().DisplaySize;

        if (std::signbit(size.x)) size.x += displaySize.x;
        if (std::signbit(size.y)) size.y += displaySize.y;

        Vec2F minSize = size, maxSize = size;

        if (flags & WindowFlag::ShrinkToFitX) minSize.x = 0.f;
        if (flags & WindowFlag::ShrinkToFitY) minSize.y = 0.f;

        imgui::SetNextWindowSizeConstraints(minSize, maxSize);

        ImGuiWindowFlags imFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
                                 | ImGuiWindowFlags_AlwaysAutoResize;

        if (flags & WindowFlag::NoTitleBar) imFlags |= ImGuiWindowFlags_NoTitleBar;
        if (flags & WindowFlag::AlwaysScrollX) imFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (flags & WindowFlag::AlwaysScrollY) imFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
        if (flags & WindowFlag::MenuBar) imFlags |= ImGuiWindowFlags_MenuBar;

        mWantDisplay = imgui::Begin(name.c_str(), nullptr, imFlags);

        // these would work, but would cause an ugly delay when the window resizes
        SQASSERT(!(std::signbit(position.x) && (flags & WindowFlag::ShrinkToFitX)), "");
        SQASSERT(!(std::signbit(position.y) && (flags & WindowFlag::ShrinkToFitY)), "");

        if (std::signbit(position.x)) position.x += displaySize.x - imgui::GetWindowSize().x;
        if (std::signbit(position.y)) position.y += displaySize.y - imgui::GetWindowSize().y;

        imgui::SetWindowPos(position);
    }

    ~scope_window_fixed() { imgui::End(); }

    //--------------------------------------------------------//

    operator bool() const { return want_display(); }

    bool want_display() const { return mWantDisplay; }

    private: bool mWantDisplay = false;
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
/// @tparam T integer type (int/uint, 8/16/32/64)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param min the minimum value to allow
/// @param max the maximum value to allow
///
/// @result was the value just modified
///
template <class T> inline
bool input_int ( string label, float labelWidth, T& ref, int step,
                 int min = std::numeric_limits<T>::min(), int max = std::numeric_limits<T>::max() )
{
    SQASSERT(min >= std::numeric_limits<T>::min(), "");
    SQASSERT(max <= std::numeric_limits<T>::max(), "");

    detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asInt32 = static_cast<int>(ref);
    bool changed = imgui::InputInt(hiddenLabel.c_str(), &asInt32, step, 0);
    ref = static_cast<T>(maths::clamp(asInt32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Float input widget with a label and step buttons.
///
/// @tparam T floating point type (float, double, long double)
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal point
/// @param min the minimum value to allow
/// @param max the maximum value to allow
///
/// @result was the value just modified
///
template <class T> inline
bool input_float ( string label, float labelWidth, T& ref, float step, int precision,
                   float min = std::numeric_limits<T>::min(), float max = std::numeric_limits<T>::max() )
{
    SQASSERT(min >= std::numeric_limits<T>::min(), "");
    SQASSERT(max <= std::numeric_limits<T>::max(), "");

    detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asFloat32 = static_cast<float>(ref);
    bool changed = imgui::InputFloat(hiddenLabel.c_str(), &asFloat32, step, 0.f, precision);
    ref = static_cast<T>(maths::clamp(asFloat32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Vector Integer input widget with a label and step buttons.
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param min the minimum value to allow
/// @param max the maximum value to allow
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

    detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asInt32 = Vector<S,int>(ref);
    bool changed;

    if constexpr (S == 2) changed = imgui::InputInt2(hiddenLabel.c_str(), asInt32.data);
    if constexpr (S == 3) changed = imgui::InputInt3(hiddenLabel.c_str(), asInt32.data);
    if constexpr (S == 4) changed = imgui::InputInt4(hiddenLabel.c_str(), asInt32.data);

    ref = Vector<S,T>(maths::clamp(asInt32, min, max));

    return changed;
}

//----------------------------------------------------------------------------//

/// Vector Float input widget with a label and step buttons.
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label
/// @param ref reference to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal points
/// @param min the minimum value to allow
/// @param max the maximum value to allow
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

    detail::display_input_label(label.c_str(), labelWidth);

    const string hiddenLabel = "##" + label;

    auto asFloat32 = Vector<S,float>(ref);
    bool changed;

    if constexpr (S == 2) changed = imgui::InputFloat2(hiddenLabel.c_str(), asFloat32.data, precision);
    if constexpr (S == 3) changed = imgui::InputFloat3(hiddenLabel.c_str(), asFloat32.data, precision);
    if constexpr (S == 4) changed = imgui::InputFloat4(hiddenLabel.c_str(), asFloat32.data, precision);

    ref = Vector<S,T>(maths::clamp(asFloat32, min, max));

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

//----------------------------------------------------------------------------//

/// Combo Box widget.
///
template <class IntType>
inline bool input_combo(string label, float labelWidth, IntType& ref, const std::vector<string>& items)
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

//============================================================================//

} // namespace sq::gui
