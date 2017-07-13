#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

#include <utility>

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
/// @endcond

namespace chai = chaiscript;
namespace rp3d = reactphysics3d;

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
