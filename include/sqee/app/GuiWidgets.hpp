#pragma once

#include <sqee/builtins.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/redist/imgui/imgui.hpp>

namespace sq::gui {

//============================================================================//

constexpr const int FONT_REGULAR = 0;
constexpr const int FONT_BOLD    = 1;
constexpr const int FONT_ITALIC  = 2;
constexpr const int FONT_MONO    = 3;

//============================================================================//

namespace detail {

bool CollapsingHeaderSized(const char* label);

void BeginFramedGroup(bool lb, bool lt, bool rb, bool rt);

void EndFramedGroup();

} // namespace detail

//============================================================================//

inline ImGuiIO& get_io() { return imgui::GetIO(); }

inline ImVector<ImFont*>& get_fonts() { return get_io().Fonts->Fonts; }

inline ImGuiStyle get_style() { return imgui::GetStyle(); }

//----------------------------------------------------------------------------//

/// Base for scopes that need to "return" a bool.
struct base_scope_result_bool
{
    operator bool() const { return mResult; }
    protected: bool mResult;
};

//============================================================================//

/// Wrapper for PushFont, PopFont.
struct scope_font
{
    scope_font(int fontIndex) { imgui::PushFont(get_fonts()[fontIndex]); }
    ~scope_font() { imgui::PopFont(); }
};

//----------------------------------------------------------------------------//

/// Wrapper for PushItemWidth, PopItemWidth.
struct scope_item_width
{
    scope_item_width(float width) { imgui::PushItemWidth(width); }
    ~scope_item_width() { imgui::PopItemWidth(); }
};

//----------------------------------------------------------------------------//

/// Draw a decorative frame around a group of widgets.
struct scope_framed_group
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
struct scope_window : base_scope_result_bool
{
    scope_window(string name, bool autoSize)
    {
        ImGuiWindowFlags flags = 0;

        if (autoSize) flags |= ImGuiWindowFlags_AlwaysAutoResize;

        mResult = imgui::Begin(name.c_str(), nullptr, flags);
    }

    ~scope_window() { imgui::End(); }
};

//----------------------------------------------------------------------------//

/// A window taking up a fixed region of the screen.
struct scope_window_fixed : base_scope_result_bool
{
    scope_window_fixed(string name, Vec2F size, Vec2F position, bool alwaysScrollH, bool alwaysScrollV)
    {
        const Vec2F displaySize = get_io().DisplaySize;

        if (std::signbit(size.x)) size.x += displaySize.x;
        if (std::signbit(size.y)) size.y += displaySize.y;

        imgui::SetNextWindowSize(size);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
                               | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

        if (alwaysScrollH) flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
        if (alwaysScrollV) flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

        imgui::Begin(name.c_str(), nullptr, flags);

        if (std::signbit(position.x)) position.x += displaySize.x - size.x;
        if (std::signbit(position.y)) position.y += displaySize.y - size.y;

        imgui::SetWindowPos(position);

        mResult = true; // can't close these yet
    }

    ~scope_window_fixed() { imgui::End(); }
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

/// Float input widget with a label to the left, and buttons to the right.
///
/// @param label the string to use for the label
/// @param labelWidth width to allocate for the label
/// @param ptr pointer to the value to manipulate
/// @param step amount to increment per +/- button click
/// @param precision digits to display after the decimal point
///
/// @result was the value just modified
///
inline bool input_float(string label, float labelWidth, float* ptr, float step, int precision)
{
    label = " " + label;

    const float cursorStart = imgui::GetCursorPosX();

    const float labelTextWidth = imgui::CalcTextSize(label.c_str()).x;
    const float labelPadding = (labelWidth - labelTextWidth) / 2.f;

    imgui::AlignFirstTextHeightToWidgets();
    imgui::SetCursorPosX(cursorStart + labelPadding - imgui::GetStyle().ItemSpacing.x);
    imgui::TextUnformatted(label.c_str());
    imgui::SameLine(cursorStart + labelWidth);

    const string inputLabel = "##" + label;
    return imgui::InputFloat(inputLabel.c_str(), ptr, step, 0.f, precision);
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
