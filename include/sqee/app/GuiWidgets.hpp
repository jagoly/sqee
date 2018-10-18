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

#define DEFINE_IMGUI_INPUT_FUNCS_INT(Type, Suffix, Format) \
inline bool InputValue(const char* label, Type* value, Type step) \
{ return InputScalar(label, ImGuiDataType_##Suffix, value, step > 0 ? &step : nullptr, nullptr, Format); }

#define DEFINE_IMGUI_INPUT_FUNCS_FLOAT(Type, Suffix) \
inline bool InputValue(const char* label, Type* value, Type step, char precision) \
{ const char format[5] = { '%', '.', precision, 'f', 0 }; \
return InputScalar(label, ImGuiDataType_##Suffix, value, step > 0 ? &step : nullptr, nullptr, format); }

DEFINE_IMGUI_INPUT_FUNCS_INT(int32_t, S32, "%d")
DEFINE_IMGUI_INPUT_FUNCS_INT(uint32_t, U32, "%u")

DEFINE_IMGUI_INPUT_FUNCS_INT(int64_t, S64, "%lld")
DEFINE_IMGUI_INPUT_FUNCS_INT(uint64_t, U64, "%llu")

DEFINE_IMGUI_INPUT_FUNCS_FLOAT(float, Float)
DEFINE_IMGUI_INPUT_FUNCS_FLOAT(double, Double)

#undef DEFINE_IMGUI_INPUT_FUNCS_INT

//----------------------------------------------------------------------------//

/// Wrapper for PushItemWidth, PopItemWidth.
struct ScopeItemWidth final
{
    ScopeItemWidth(float width) { ImGui::PushItemWidth(width); }
    ~ScopeItemWidth() { ImGui::PopItemWidth(); }
};

/// Wrapper for Indent, Unindent.
struct ScopeIndent final
{
    ScopeIndent() { ImGui::Indent(); }
    ~ScopeIndent() { ImGui::Unindent(); }
};

/// Wrapper for Unindent, Indent.
struct ScopeUnindent final
{
    ScopeUnindent() { ImGui::Unindent(); }
    ~ScopeUnindent() { ImGui::Indent(); }
};

/// Wrapper for PushFont, PopFont.
struct ScopeFont final
{
    ScopeFont(int fontIndex) { ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[fontIndex]); }
    ~ScopeFont() { ImGui::PopFont(); }
};

//============================================================================//

} // namespace ImGui
