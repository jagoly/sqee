#pragma once

#include <array>
#include <deque>
#include <string>
#include <vector>
#include <memory>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
using std::pair;
using std::array;
using std::deque;
using std::string;
using std::vector;
using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;
using std::forward_list;
using std::unordered_set;
using std::unordered_map;
using std::initializer_list;

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
using glm::fvec2; using glm::fvec3; using glm::fvec4;
using glm::dvec2; using glm::dvec3; using glm::dvec4;
using glm::ivec2; using glm::ivec3; using glm::ivec4;
using glm::uvec2; using glm::uvec3; using glm::uvec4;
using glm::fmat2; using glm::fmat3; using glm::fmat4;
using glm::dmat2; using glm::dmat3; using glm::dmat4;
using glm::fquat; using glm::dquat;

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

template<class T, class U>
weak_ptr<T> static_wptr_cast(weak_ptr<U> const& _wptr) {
    return std::static_pointer_cast<T>(shared_ptr<U>(_wptr));
}

template<class T>
bool wptr_expired(weak_ptr<T> const& _wptr) {
    return _wptr.expired();
}

namespace sq {

class Scene;
class Handler;
class Application;
class SettingsMaps;
class PreProcessor;
class SoundManager;

class Camera;
class UboCamera;
class Mesh;
class Skin;
class Animation;
class Skeleton;

class Texture2D;
class Texture2DArray;
class TextureCube;
class Framebuffer;
class Preprocessor;
class Shader;
class Pipeline;
class Uniformbuffer;

template <class Tk, class Tv>
class IndexedMap;

template <class T>
class ResHolder;

enum class Direction { None, North, East, South, West };
enum class Alignment { TL, TC, TR, CL, CC, CR, BL, BC, BR };
enum class Grid4x4 { AA, AB, AC, AD, BA, BB, BC, BD, CA, CB, CC, CD, DA, DB, DC, DD };
struct Plane { dvec3 normal; double offset; };
struct Sphere { dvec3 origin; double radius; };
struct Frustum { Sphere sphere; Plane pN, pT, pB, pL, pR; dvec3 xy, xY, Xy, XY; };
struct BoundBox { Sphere sphere; dvec3 size, nX{1,0,0}, nY{0,1,0}, nZ{0,0,1}; };

}
