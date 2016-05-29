#pragma once

#include <cmath>

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

struct expand {
    template<class... Args> expand(Args&&...) {}
};

}

namespace chaiscript {}
namespace reactphysics3d {}
namespace chai = chaiscript;
namespace rp3d = reactphysics3d;
