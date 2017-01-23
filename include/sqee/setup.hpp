#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

#include <type_traits>
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

namespace sq {

//============================================================================//

struct NonCopyable
{
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};

struct MoveOnly
{
    MoveOnly() = default;

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

//============================================================================//

/// variadic function call expansion helper
struct expand { template <class... Args> expand(Args&&...) {} };

/// returns its argument as a const reference
template <class Type> constexpr std::add_const_t<Type>& as_const(Type& arg) { return arg; }

//============================================================================//

} // namespace sq

//============================================================================//

namespace chaiscript {}
namespace reactphysics3d {}

namespace chai = chaiscript;
namespace rp3d = reactphysics3d;
