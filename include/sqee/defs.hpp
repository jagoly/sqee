#pragma once

#ifndef SQ_TEXTURES
#define SQ_TEXTURES "res/textures/"
#endif

#ifndef SQ_MODELS
#define SQ_MODELS "res/models/"
#endif

#ifndef SQ_SHADERS
#define SQ_SHADERS "res/shaders/"
#endif

#ifndef SQ_FONTS
#define SQ_FONTS "res/fonts/"
#endif

#include <iostream>
#include <cstring>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <initializer_list>

using std::cout;
using std::endl;
using std::array;
using std::list;
using std::map;
using std::unordered_map;
using std::pair;
using std::string;
using std::set;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

typedef unsigned int    uint;
typedef unsigned short  ushort;
typedef unsigned char   uchar;

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef void            GLvoid;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef int             GLsizei;
typedef float           GLfloat;
typedef float           GLclampf;
typedef double          GLdouble;
typedef double          GLclampd;
typedef char            GLchar;

const size_t charSz   = sizeof(char);
const size_t shortSz  = sizeof(short);
const size_t intSz    = sizeof(int);
const size_t floatSz  = sizeof(float);
const size_t doubleSz = sizeof(double);

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

template <typename T>
bool val_in(const T& _val, const std::initializer_list<T>& _in) {
    return std::find(_in.begin(), _in.end(), _val) != _in.end();
}

namespace sq {
    enum class Direction { Zero, North, East, South, West };
}

namespace std {
inline unsigned int stou(const string& _str, size_t* _idx = 0, int _base = 10) {
    return (unsigned int)stoul(_str, _idx, _base);
}
}

using std::stoi; using std::stof; using std::stou;
