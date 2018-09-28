#include <sqee/redist/tinyformat.hpp>

#include <sqee/misc/StringCast.hpp>

namespace sq {

//============================================================================//

// Chai Vector2 /////

String chai_string(const Vec2I& arg)
{ return tfm::format("Vec2I(%i, %i)", arg.x, arg.y); }
String chai_string(const Vec2U& arg)
{ return tfm::format("Vec2U(%u, %u)", arg.x, arg.y); }
String chai_string(const Vec2F& arg)
{ return tfm::format("Vec2F(%f, %f)", arg.x, arg.y); }

// GLSL Vector2 /////

String glsl_string(const Vec2I& arg)
{ return tfm::format("ivec2(%i, %i)", arg.x, arg.y); }
String glsl_string(const Vec2U& arg)
{ return tfm::format("uvec2(%u, %u)", arg.x, arg.y); }
String glsl_string(const Vec2F& arg)
{ return tfm::format("fvec2(%f, %f)", arg.x, arg.y); }

//============================================================================//

// Chai Vector3 /////

String chai_string(const Vec3I& arg)
{ return tfm::format("Vec3I(%i, %i, %i)", arg.x, arg.y, arg.z); }
String chai_string(const Vec3U& arg)
{ return tfm::format("Vec3U(%u, %u, %u)", arg.x, arg.y, arg.z); }
String chai_string(const Vec3F& arg)
{ return tfm::format("Vec3F(%f, %f, %f)", arg.x, arg.y, arg.z); }

// GLSL Vector3 /////

String glsl_string(const Vec3I& arg)
{ return tfm::format("ivec3(%i, %i, %i)", arg.x, arg.y, arg.z); }
String glsl_string(const Vec3U& arg)
{ return tfm::format("uvec3(%u, %u, %u)", arg.x, arg.y, arg.z); }
String glsl_string(const Vec3F& arg)
{ return tfm::format("fvec3(%f, %f, %f)", arg.x, arg.y, arg.z); }

//============================================================================//

// Chai Vector4 /////

String chai_string(const Vec4I& arg)
{ return tfm::format("Vec4I(%i, %i, %i, %i)", arg.x, arg.y, arg.z, arg.w); }
String chai_string(const Vec4U& arg)
{ return tfm::format("Vec4U(%u, %u, %u, %u)", arg.x, arg.y, arg.z, arg.w); }
String chai_string(const Vec4F& arg)
{ return tfm::format("Vec4F(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

// GLSL Vector4 /////

String glsl_string(const Vec4I& arg)
{ return tfm::format("ivec4(%i, %i, %i, %i)", arg.x, arg.y, arg.z, arg.w); }
String glsl_string(const Vec4U& arg)
{ return tfm::format("uvec4(%u, %u, %u, %u)", arg.x, arg.y, arg.z, arg.w); }
String glsl_string(const Vec4F& arg)
{ return tfm::format("fvec4(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

//============================================================================//

// Chai Float Matrix3x3 /////

String chai_string(const Mat3F& arg)
{
    return tfm::format (
        "Mat3F((%f, %f, %f), (%f, %f, %f), (%f, %f, %f))",
        arg[0][0], arg[0][1], arg[0][2],
        arg[1][0], arg[1][1], arg[1][2],
        arg[2][0], arg[2][1], arg[2][2]
    );
}

//============================================================================//

// Chai Float Matrix3x4 /////

String chai_string(const Mat34F& arg)
{
    return tfm::format (
        "Mat3F((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
        arg[0][0], arg[0][1], arg[0][2], arg[0][3],
        arg[1][0], arg[1][1], arg[1][2], arg[1][3],
        arg[2][0], arg[2][1], arg[2][2], arg[2][3]
    );
}

//============================================================================//

// Chai Float Matrix4x4 /////

String chai_string(const Mat4F& arg)
{
    return tfm::format (
        "Mat4F((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
        arg[0][0], arg[0][1], arg[0][2], arg[0][3],
        arg[1][0], arg[1][1], arg[1][2], arg[1][3],
        arg[2][0], arg[2][1], arg[2][2], arg[2][3],
        arg[3][0], arg[3][1], arg[3][2], arg[3][3]
    );
}

//============================================================================//

// Chai Float Quaternion /////

String chai_string(const QuatF& arg)
{ return tfm::format("QuatF(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

// GLSL Float Quaternion /////

String glsl_string(const QuatF& arg)
{ return tfm::format("fvec4(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

//============================================================================//

} // namespace sq
