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

typedef unsigned int ImGuiID;

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

template <class T> constexpr const char* const TypeName = nullptr;

template <> inline const char* const TypeName<int8_t> = "Int8";
template <> inline const char* const TypeName<uint8_t> = "UInt8";
template <> inline const char* const TypeName<int16_t> = "Int16";
template <> inline const char* const TypeName<uint16_t> = "UInt16";
template <> inline const char* const TypeName<int32_t> = "Int32";
template <> inline const char* const TypeName<uint32_t> = "UInt32";
template <> inline const char* const TypeName<int64_t> = "Int64";
template <> inline const char* const TypeName<uint64_t> = "UInt64";
template <> inline const char* const TypeName<float> = "Float";
template <> inline const char* const TypeName<double> = "Double";

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

/// @namespace sq::traits
/// @brief Helper traits for various types.
