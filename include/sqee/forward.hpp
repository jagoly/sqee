#pragma once

#include <glm/fwd.hpp>
#include <string>

using std::string;

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

class BaseCamera;
class LookatCamera;

class Texture;
class Framebuffer;
class Shader;
class Pipeline;
class UniformBuffer;

template <class Tk, class Tv>
class IndexedMap;

template <class T>
class ResHolder;

enum class Direction { Zero, North, East, South, West };
enum class Alignment { TL, TC, TR, CL, CC, CR, BL, BC, BR };

}
