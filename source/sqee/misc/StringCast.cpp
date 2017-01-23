#include <sqee/redist/tinyformat.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/misc/StringCast.hpp>

namespace sq {

//============================================================================//

// Chai Scalar /////

template<> std::string chai_string<int>(const int& arg)
{ return tfm::format("int(%i)", arg); }
template<> std::string chai_string<uint>(const uint& arg)
{ return tfm::format("uint(%u)", arg); }
template<> std::string chai_string<float>(const float& arg)
{ return tfm::format("float(%f)", arg); }

//============================================================================//

// Chai Vector2 /////

template<> std::string chai_string<Vec2I>(const Vec2I& arg)
{ return tfm::format("Vec2I(%i, %i)", arg.x, arg.y); }
template<> std::string chai_string<Vec2U>(const Vec2U& arg)
{ return tfm::format("Vec2U(%u, %u)", arg.x, arg.y); }
template<> std::string chai_string<Vec2F>(const Vec2F& arg)
{ return tfm::format("Vec2F(%f, %f)", arg.x, arg.y); }

// GLSL Vector2 /////

template<> std::string glsl_string<Vec2I>(const Vec2I& arg)
{ return tfm::format("ivec2(%i, %i)", arg.x, arg.y); }
template<> std::string glsl_string<Vec2U>(const Vec2U& arg)
{ return tfm::format("uvec2(%u, %u)", arg.x, arg.y); }
template<> std::string glsl_string<Vec2F>(const Vec2F& arg)
{ return tfm::format("fvec2(%f, %f)", arg.x, arg.y); }

//============================================================================//

// Chai Vector3 /////

template<> std::string chai_string<Vec3I>(const Vec3I& arg)
{ return tfm::format("Vec3I(%i, %i, %i)", arg.x, arg.y, arg.z); }
template<> std::string chai_string<Vec3U>(const Vec3U& arg)
{ return tfm::format("Vec3U(%u, %u, %u)", arg.x, arg.y, arg.z); }
template<> std::string chai_string<Vec3F>(const Vec3F& arg)
{ return tfm::format("Vec3F(%f, %f, %f)", arg.x, arg.y, arg.z); }

// GLSL Vector3 /////

template<> std::string glsl_string<Vec3I>(const Vec3I& arg)
{ return tfm::format("ivec3(%i, %i, %i)", arg.x, arg.y, arg.z); }
template<> std::string glsl_string<Vec3U>(const Vec3U& arg)
{ return tfm::format("uvec3(%u, %u, %u)", arg.x, arg.y, arg.z); }
template<> std::string glsl_string<Vec3F>(const Vec3F& arg)
{ return tfm::format("fvec3(%f, %f, %f)", arg.x, arg.y, arg.z); }

//============================================================================//

// Chai Vector4 /////

template<> std::string chai_string<Vec4I>(const Vec4I& arg)
{ return tfm::format("Vec4I(%i, %i, %i, %i)", arg.x, arg.y, arg.z, arg.w); }
template<> std::string chai_string<Vec4U>(const Vec4U& arg)
{ return tfm::format("Vec4U(%u, %u, %u, %u)", arg.x, arg.y, arg.z, arg.w); }
template<> std::string chai_string<Vec4F>(const Vec4F& arg)
{ return tfm::format("Vec4F(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

// GLSL Vector4 /////

template<> std::string glsl_string<Vec4I>(const Vec4I& arg)
{ return tfm::format("ivec4(%i, %i, %i, %i)", arg.x, arg.y, arg.z, arg.w); }
template<> std::string glsl_string<Vec4U>(const Vec4U& arg)
{ return tfm::format("uvec4(%u, %u, %u, %u)", arg.x, arg.y, arg.z, arg.w); }
template<> std::string glsl_string<Vec4F>(const Vec4F& arg)
{ return tfm::format("fvec4(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

//============================================================================//

// Chai Float Matrix2x2 /////

template<> std::string chai_string<Mat2F>(const Mat2F& arg)
{
    return tfm::format (
        "Mat2F((%f, %f), (%f, %f))",
        arg[0][0], arg[0][1],
        arg[1][0], arg[1][1]
    );
}

//============================================================================//

// Chai Float Matrix2x3 /////

template<> std::string chai_string<Mat23F>(const Mat23F& arg)
{
    return tfm::format (
        "Mat23F((%f, %f, %f), (%f, %f, %f))",
        arg[0][0], arg[0][1], arg[0][2],
        arg[1][0], arg[1][1], arg[1][2]
    );
}

//============================================================================//

// Chai Float Matrix3x3 /////

template<> std::string chai_string<Mat3F>(const Mat3F& arg)
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

template<> std::string chai_string<Mat34F>(const Mat34F& arg)
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

template<> std::string chai_string<Mat4F>(const Mat4F& arg)
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

template<> std::string chai_string<QuatF>(const QuatF& arg)
{ return tfm::format("QuatF(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

// GLSL Float Quaternion /////

template<> std::string glsl_string<QuatF>(const QuatF& arg)
{ return tfm::format("fvec4(%f, %f, %f, %f)", arg.x, arg.y, arg.z, arg.w); }

//============================================================================//

} // namespace sq
