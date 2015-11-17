#include <sqee/redist/tinyformat.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/misc/StringCast.hpp>

namespace sq {

// Chai Scalar /////
template<> std::string chai_string<int>(const int& _val) {
    return tfm::format("int(%i)", _val); }
template<> std::string chai_string<uint>(const uint& _val) {
    return tfm::format("uint(%u)", _val); }
template<> std::string chai_string<float>(const float& _val) {
    return tfm::format("float(%f)", _val); }

// Chai Vector2 /////
template<> std::string chai_string<Vec2I>(const Vec2I& _val) {
    return tfm::format("Vec2I(%i, %i)", _val.x, _val.y); }
template<> std::string chai_string<Vec2U>(const Vec2U& _val) {
    return tfm::format("Vec2U(%u, %u)", _val.x, _val.y); }
template<> std::string chai_string<Vec2B>(const Vec2B& _val) {
    return tfm::format("Vec2B(%b, %b)", _val.x, _val.y); }
template<> std::string chai_string<Vec2F>(const Vec2F& _val) {
    return tfm::format("Vec2F(%f, %f)", _val.x, _val.y); }

// GLSL Vector2 /////
template<> std::string glsl_string<Vec2I>(const Vec2I& _val) {
    return tfm::format("ivec2(%i, %i)", _val.x, _val.y); }
template<> std::string glsl_string<Vec2U>(const Vec2U& _val) {
    return tfm::format("uvec2(%u, %u)", _val.x, _val.y); }
template<> std::string glsl_string<Vec2F>(const Vec2F& _val) {
    return tfm::format("vec2(%f, %f)", _val.x, _val.y); }

// Chai Vector3 /////
template<> std::string chai_string<Vec3I>(const Vec3I& _val) {
    return tfm::format("Vec3I(%i, %i, %i)", _val.x, _val.y, _val.z); }
template<> std::string chai_string<Vec3U>(const Vec3U& _val) {
    return tfm::format("Vec3U(%u, %u, %u)", _val.x, _val.y, _val.z); }
template<> std::string chai_string<Vec3B>(const Vec3B& _val) {
    return tfm::format("Vec3B(%b, %b, %b)", _val.x, _val.y, _val.z); }
template<> std::string chai_string<Vec3F>(const Vec3F& _val) {
    return tfm::format("Vec3F(%f, %f, %f)", _val.x, _val.y, _val.z); }

// GLSL Vector3 /////
template<> std::string glsl_string<Vec3I>(const Vec3I& _val) {
    return tfm::format("ivec3(%i, %i)", _val.x, _val.y, _val.z); }
template<> std::string glsl_string<Vec3U>(const Vec3U& _val) {
    return tfm::format("uvec3(%u, %u)", _val.x, _val.y, _val.z); }
template<> std::string glsl_string<Vec3F>(const Vec3F& _val) {
    return tfm::format("vec3(%f, %f)", _val.x, _val.y, _val.z); }

// Chai Vector4 /////
template<> std::string chai_string<Vec4I>(const Vec4I& _val) {
    return tfm::format("Vec4I(%i, %i, %i, %i)", _val.x, _val.y, _val.z, _val.w); }
template<> std::string chai_string<Vec4U>(const Vec4U& _val) {
    return tfm::format("Vec4U(%u, %u, %u, %u)", _val.x, _val.y, _val.z, _val.w); }
template<> std::string chai_string<Vec4B>(const Vec4B& _val) {
    return tfm::format("Vec4B(%b, %b, %b, %b)", _val.x, _val.y, _val.z, _val.w); }
template<> std::string chai_string<Vec4F>(const Vec4F& _val) {
    return tfm::format("Vec4F(%f, %f, %f, %f)", _val.x, _val.y, _val.z, _val.w); }

// GLSL Vector4 /////
template<> std::string glsl_string<Vec4I>(const Vec4I& _val) {
    return tfm::format("ivec4(%i, %i, %i, %i)", _val.x, _val.y, _val.z, _val.w); }
template<> std::string glsl_string<Vec4U>(const Vec4U& _val) {
    return tfm::format("uvec4(%u, %u, %u, %u)", _val.x, _val.y, _val.z, _val.w); }
template<> std::string glsl_string<Vec4F>(const Vec4F& _val) {
    return tfm::format("vec4(%f, %f, %f, %f)", _val.x, _val.y, _val.z, _val.w); }

// Chai Float Matrix3x3 /////
template<> std::string chai_string<Mat3F>(const Mat3F& _val) {
    return tfm::format("Mat3F((%f, %f, %f), (%f, %f, %f), (%f, %f, %f))",
                       _val[0][0], _val[0][1], _val[0][2],
                       _val[1][0], _val[1][1], _val[1][2],
                       _val[2][0], _val[2][1], _val[2][2]);
}

// GLSL Float Matrix3x3 /////
template<> std::string glsl_string<Mat3F>(const Mat3F& _val) {
    return tfm::format("fmat3(%f, %f, %f, %f, %f, %f, %f, %f, %f)",
                       _val[0][0], _val[0][1], _val[0][2],
                       _val[1][0], _val[1][1], _val[1][2],
                       _val[2][0], _val[2][1], _val[2][2]);
}

// Chai Float Matrix4x4 /////
template<> std::string chai_string<Mat4F>(const Mat4F& _val) {
    return tfm::format("Mat4F((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
                       _val[0][0], _val[0][1], _val[0][2], _val[0][3],
                       _val[1][0], _val[1][1], _val[1][2], _val[1][3],
                       _val[2][0], _val[2][1], _val[2][2], _val[2][3],
                       _val[3][0], _val[3][1], _val[3][2], _val[3][3]);
}

// GLSL Float Matrix4x4 /////
template<> std::string glsl_string<Mat4F>(const Mat4F& _val) {
    return tfm::format("fmat4(%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)",
                       _val[0][0], _val[0][1], _val[0][2], _val[0][3],
                       _val[1][0], _val[1][1], _val[1][2], _val[1][3],
                       _val[2][0], _val[2][1], _val[2][2], _val[2][3],
                       _val[3][0], _val[3][1], _val[3][2], _val[3][3]);
}

// Chai Float Quaternion /////
template<> std::string chai_string<QuatF>(const QuatF& _val) {
    return tfm::format("QuatF(%f, %f, %f, %f)", _val.x, _val.y, _val.z, _val.w);
}

// GLSL Float Quaternion /////
template<> std::string glsl_string<QuatF>(const QuatF& _val) {
    return tfm::format("vec4(%f, %f, %f, %f)", _val.x, _val.y, _val.z, _val.w);
}

}
