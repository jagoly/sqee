#pragma once

#include <cmath>
#include <cassert>

#ifdef SQEE_DEBUG
#define SQDEBUG(code) code
#define SQASSERT(code) assert(code)
#else
#define SQDEBUG(code)
#define SQASSERT(code)
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef signed int    GLint;
typedef unsigned int  GLuint;
typedef float         GLfloat;
typedef double        GLdouble;
typedef unsigned int  GLbitfield;
typedef signed int    GLsizei;
typedef unsigned int  GLenum;
typedef char          GLchar;

namespace sq {

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}

namespace chaiscript {}
namespace reactphysics3d {}
namespace chai = chaiscript;
namespace rp3d = reactphysics3d;
