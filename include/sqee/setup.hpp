#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

#include <utility>

#include <sqee/export.hpp>

//============================================================================//

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

typedef signed int    GLint;
typedef unsigned int  GLuint;
typedef float         GLfloat;
typedef double        GLdouble;
typedef unsigned int  GLbitfield;
typedef signed int    GLsizei;
typedef unsigned int  GLenum;
typedef char          GLchar;

//============================================================================//

/// @cond ignore
namespace chaiscript {}
namespace reactphysics3d {}
namespace ImGui {}
/// @endcond

namespace chai = chaiscript;
namespace rp3d = reactphysics3d;
namespace imgui = ImGui;

//============================================================================//

namespace sq {

//============================================================================//

/// Base for objects that can't be copied or moved.
struct NonCopyable
{
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};

//----------------------------------------------------------------------------//

/// Base for objects that can be moved but not copied.
struct MoveOnly
{
    MoveOnly() = default;

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

//----------------------------------------------------------------------------//

namespace traits {

template <class T> const char* TypeName;

template <> constexpr const char* TypeName<int8_t> = "Int8";
template <> constexpr const char* TypeName<uint8_t> = "UInt8";
template <> constexpr const char* TypeName<int16_t> = "Int16";
template <> constexpr const char* TypeName<uint16_t> = "UInt16";
template <> constexpr const char* TypeName<int32_t> = "Int32";
template <> constexpr const char* TypeName<uint32_t> = "UInt32";
template <> constexpr const char* TypeName<int64_t> = "Int64";
template <> constexpr const char* TypeName<uint64_t> = "UInt64";
template <> constexpr const char* TypeName<float> = "Float";
template <> constexpr const char* TypeName<double> = "Double";

} // namespace traits

//============================================================================//

} // namespace sq

//============================================================================//

/// @namespace sq
/// @brief The primary SQEE namespace.

/// @namespace sq::algo
/// @brief Some very basic generic algorithms.

/// @namespace sq::dop
/// @brief Utilities for data oriented programming.

/// @namespace sq::maths
/// @brief Mathematical classes and functions.

/// @namespace sq::gui
/// @brief Helper classes and functions for ImGui.
