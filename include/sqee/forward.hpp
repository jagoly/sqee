#pragma once

#include <string>
#include <array>
#include <memory>
using std::string;
using std::array;
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>

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

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

template <class T>
struct AlwaysTrue {
    bool operator()(const T&) const { return true; }
};

namespace sq {

class Scene;
class Handler;
class SettingMap;
class SettingValue;
class Application;
class SoundManager;

class Mesh;
class Animation;
class Skeleton;
class Skin;

class Camera;

class Texture;
class Framebuffer;
class FramebufferRaw;
class Shader;
class Pipeline;
class UniformBuffer;

template <class Tk, class Tv>
class IndexedMap;

template <class T>
class ResHolder;

enum class Direction { Zero, North, East, South, West };
enum class Alignment { TL, TC, TR, CL, CC, CR, BL, BC, BR };
enum class Grid4x4 { AA, AB, AC, AD, BA, BB, BC, BD, CA, CB, CC, CD, DA, DB, DC, DD };

}
