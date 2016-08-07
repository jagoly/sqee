#pragma once

#include <cmath>
#include <iterator>

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

template<class T> struct reversion_wrapper { T& iterable; };
template<class T> reversion_wrapper<T> reverse(T&& iterable) { return {iterable}; }
template<class T> auto begin(reversion_wrapper<T> _wrapper) { return std::rbegin(_wrapper.iterable); }
template<class T> auto end(reversion_wrapper<T> _wrapper) { return std::rend(_wrapper.iterable); }

}

namespace chaiscript {}
namespace reactphysics3d {}
namespace chai = chaiscript;
namespace rp3d = reactphysics3d;
