//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
// This file contains compile-time options for ImGui.
// Other options (memory allocation overrides, callbacks, etc.) can be set at runtime via the ImGuiIO structure - ImGui::GetIO().
//-----------------------------------------------------------------------------

#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Builtins.hpp>

#include <sqee/export.hpp>

//---- Define assertion handler. Defaults to calling assert().

#define IM_ASSERT(Expr) SQASSERT(Expr, "")

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.

#define IMGUI_API SQEE_API

//---- Don't implement default handlers for Windows (so as not to link with OpenClipboard() and others Win32 functions)

#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS
#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCS

//---- Don't define obsolete functions names. Consider enabling from time to time or when updating to reduce like hood of using already obsolete function/names

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.

#define IM_VEC2_CLASS_EXTRA \
    ImVec2(sq::Vec2F v) : x(v.x), y(v.y) {} \
    operator sq::Vec2F() const { return { x, y }; }

#define IM_VEC4_CLASS_EXTRA \
    ImVec4(sq::Vec4F v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
    operator sq::Vec4F() const { return { x, y, z, w }; }
