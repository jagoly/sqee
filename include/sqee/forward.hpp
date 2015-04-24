#pragma once

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
using std::pair;
using std::array;
using std::string;
using std::vector;
using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;
using std::forward_list;
using std::unordered_set;
using std::unordered_map;

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
using glm::vec2; using glm::vec3; using glm::vec4;
using glm::dvec2; using glm::dvec3; using glm::dvec4;
using glm::ivec2; using glm::ivec3; using glm::ivec4;
using glm::uvec2; using glm::uvec3; using glm::uvec4;
using glm::mat2; using glm::mat2x3; using glm::mat2x4;
using glm::mat3x2; using glm::mat3; using glm::mat3x4;
using glm::mat4x2; using glm::mat4x3; using glm::mat4;
using glm::dmat2; using glm::dmat2x3; using glm::dmat2x4;
using glm::dmat3x2; using glm::dmat3; using glm::dmat3x4;
using glm::dmat4x2; using glm::dmat4x3; using glm::dmat4;

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

namespace sq {

class Scene;
class Handler;
class SettingMap;
class Application;
class SoundManager;

class Camera;
class UboCamera;
class Mesh;
class Skin;
class Animation;
class Skeleton;
class LightSky;
class LightSpot;
class LightPoint;

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
struct Frustum { dvec3 xyz, xyZ, xYz, xYZ, Xyz, XyZ, XYz, XYZ; dvec4 pT, pB, pL, pR, pN, pF; };
struct Sphere { dvec3 origin; double radius; };
struct BoundBox { dvec3 origin, size; double radius; dvec3 nX{1,0,0}, nY{0,1,0}, nZ{0,0,1}; };

}
