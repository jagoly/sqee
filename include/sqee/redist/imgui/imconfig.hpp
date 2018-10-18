//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR IMGUI
// Runtime options (clipboard callbacks, enabling various features, etc.) can generally be set via the ImGuiIO structure.
// You can use ImGui::SetAllocatorFunctions() before calling ImGui::CreateContext() to rewire memory allocation functions.
//-----------------------------------------------------------------------------

#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/maths/Builtins.hpp>

#include <sqee/export.hpp>

//---- Define assertion handler. Defaults to calling assert().
#define IM_ASSERT(Expr) SQASSERT(Expr, "")

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows.
#define IMGUI_API SQEE_API

//---- Don't define obsolete functions/enums names. Consider enabling from time to time after updating to avoid using soon-to-be obsolete function/names.
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//---- Don't implement demo windows functionality (ShowDemoWindow()/ShowStyleEditor()/ShowUserGuide() methods will be empty)
//---- It is very strongly recommended to NOT disable the demo windows during development. Please read the comments in imgui_demo.cpp.
//#define IMGUI_DISABLE_DEMO_WINDOWS

//---- Don't implement some functions to reduce linkage requirements.
#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS   // [Win32] Don't implement default clipboard handler. Won't use and link with OpenClipboard/GetClipboardData/CloseClipboard etc.
#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS         // [Win32] Don't implement default IME handler. Won't use and link with ImmGetContext/ImmSetCompositionWindow.
//#define IMGUI_DISABLE_FORMAT_STRING_FUNCTIONS             // Don't implement ImFormatString/ImFormatStringV so you can implement them yourself if you don't want to link with vsnprintf.
//#define IMGUI_DISABLE_MATH_FUNCTIONS                      // Don't implement ImFabs/ImSqrt/ImPow/ImFmod/ImCos/ImSin/ImAcos/ImAtan2 wrapper so you can implement them yourself. Declare your prototypes in imconfig.h.
//#define IMGUI_DISABLE_DEFAULT_ALLOCATORS                  // Don't implement default allocators calling malloc()/free() to avoid linking with them. You will need to call ImGui::SetAllocatorFunctions().

//---- Include imgui_user.h at the end of imgui.h as a convenience
//#define IMGUI_INCLUDE_IMGUI_USER_H

//---- Pack colors to BGRA8 instead of RGBA8 (to avoid converting from one to another)
//#define IMGUI_USE_BGRA_PACKED_COLOR

//---- Define constructor and implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.

#define IM_VEC2_CLASS_EXTRA \
    ImVec2(sq::Vec2F v) : x(v.x), y(v.y) {} \
    operator sq::Vec2F() const { return { x, y }; }

#define IM_VEC4_CLASS_EXTRA \
    ImVec4(sq::Vec4F v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
    operator sq::Vec4F() const { return { x, y, z, w }; }
