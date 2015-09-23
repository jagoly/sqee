#pragma once

#include <list>
#include <array>
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>
#include <functional>

using std::pair;
using std::list;
using std::array;
using std::deque;
using std::vector;
using std::string;
using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;
using std::forward_list;
using std::unordered_map;
using std::unordered_set;
using std::runtime_error;
using std::function;

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x3.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::ivec2; using glm::ivec3; using glm::ivec4;
using glm::uvec2; using glm::uvec3; using glm::uvec4;
using glm::fvec2; using glm::fvec3; using glm::fvec4;
using glm::dvec2; using glm::dvec3; using glm::dvec4;
using glm::fmat2; using glm::fmat3; using glm::fmat4;
using glm::dmat2; using glm::dmat3; using glm::dmat4;
using glm::fmat3x4; using glm::fmat4x3;
using glm::dmat3x4; using glm::dmat4x3;
using glm::fquat; using glm::dquat;

#include <algorithm>
#include <glm/common.hpp>

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

class Application;
class PreProcessor;
class SoundManager;
class Settings;
class Handler;
class Scene;

class ChaiConsole;
class DebugOverlay;

class Armature;
class ArmatureBone;
class ArmaTransform;
class PhysObject;
class Camera;
class Mesh;
class Skin;

class FrameBuffer;
class UniformBuffer;
class Pipeline;
class Shader;

class Texture2D;
class TextureMut2D;
class TextureCube;
class TextureMutCube;
class Texture2DArray;
class TextureMut2DArray;
class TextureCubeArray;
class TextureMutCubeArray;
class TextureVolume;

struct Plane { fvec3 normal; float offset; };
struct Sphere { fvec3 origin; float radius; };
struct Frustum { Sphere sphere; Plane pN, pT, pB, pL, pR; fvec3 xy, xY, Xy, XY; };
struct BoundBox { Sphere sphere; fvec3 size, nX{1,0,0}, nY{0,1,0}, nZ{0,0,1}; };
struct OrthoFrus { Plane pT, pB, pL, pR; };

}

#ifdef SQEE_DEBUG
#define SQDEBUG(code) code
#else
#define SQDEBUG(code)
#endif
