#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq::maths { //########################################################

/// Apply a scale to a 3x3 matrix.
template <class T>
constexpr Matrix33<T> scale(Matrix33<T> m, Vector3<T> v)
{
    return { m[0] * v.x, m[1] * v.y, m[2] * v.z };
}

/// Apply a scale to a 4x4 matrix.
template <class T>
constexpr Matrix44<T> scale(Matrix44<T> m, Vector3<T> v)
{
    return { m[0] * v.x, m[1] * v.y, m[2] * v.z, m[3] };
}

/// Apply a translation to a 4x4 matrix.
template <class T>
constexpr Matrix44<T> translate(Matrix44<T> m, Vector3<T> v)
{
    Vector colD = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
    return { m[0], m[1], m[2], colD };
}

//==============================================================================

/// Create a matrix from a translation, rotation, and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation, Vector3<T> scale)
{
    Vector colA { rotation[0] * scale.x, T(0.0) };
    Vector colB { rotation[1] * scale.y, T(0.0) };
    Vector colC { rotation[2] * scale.z, T(0.0) };
    Vector colD { translation, T(1.0) };

    return { colA, colB, colC, colD };
}

/// Create a matrix from a translation, rotation, and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation, T scale)
{
    return maths::transform(translation, rotation, Vector3<T>(scale));
}

/// Create a matrix from a translation, rotation, and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, Vector3<T> scale)
{
    return maths::transform(translation, Matrix33<T>(rotation), scale);
}

/// Create a matrix from a translation, rotation, and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, T scale)
{
    return maths::transform(translation, Matrix33<T>(rotation), Vector3<T>(scale));
}

//==============================================================================

/// Create a matrix from a translation and rotation.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation)
{
    Vector colA { rotation[0], T(0.0) };
    Vector colB { rotation[1], T(0.0) };
    Vector colC { rotation[2], T(0.0) };
    Vector colD { translation, T(1.0) };

    return { colA, colB, colC, colD };
}

/// Create a matrix from a translation and rotation.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation)
{
    return maths::transform(translation, Matrix33<T>(rotation));
}

//==============================================================================

/// Create a matrix from a translation and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, Vector3<T> scale)
{
    Vector colA { scale.x, T(0.0), T(0.0), T(0.0) };
    Vector colB { T(0.0), scale.y, T(0.0), T(0.0) };
    Vector colC { T(0.0), T(0.0), scale.z, T(0.0) };
    Vector colD { translation, T(1.0) };

    return { colA, colB, colC, colD };
}

/// Create a matrix from a translation and scale.
template <class T>
constexpr Matrix44<T> transform(Vector3<T> translation, T scale)
{
    return maths::transform(translation, Vector3<T>(scale));
}

//==============================================================================

/// Create a rotation matrix from an axis and angle.
template <class T>
inline Matrix33<T> rotation(Vector3<T> axis, T angle)
{
    T c = std::cos(maths::radians(angle));
    Vector s = axis * std::sin(maths::radians(angle));

    Vector x = axis * axis.x, y = axis * axis.y, z = axis * axis.z;

    Vector colA { x.x + (T(1.0) - x.x) * c, x.y * (T(1.0) - c) - s.z, x.z * (T(1.0) - c) + s.y };
    Vector colB { y.x * (T(1.0) - c) + s.z, y.y + (T(1.0) - y.y) * c, y.z * (T(1.0) - c) - s.x };
    Vector colC { z.x * (T(1.0) - c) - s.y, z.y * (T(1.0) - c) + s.x, z.z + (T(1.0) - z.z) * c };

    return { colA, colB, colC };
}

/// Create a rotation matrix from an Euler rotation.
template <class T>
inline Matrix33<T> rotation(Vector3<T> xyz)
{
    Vector c = maths::cos(-xyz);
    Vector s = maths::sin(-xyz);

    Vector colA { c.y * c.z, -c.x * s.z + s.x * s.y * c.z, s.x * s.z + c.x * s.y * c.z };
    Vector colB { c.y * s.z, c.x * c.z + s.x * s.y * s.z, -s.x * c.z + c.x * s.y * s.z };
    Vector colC { -s.y, s.x * c.y, c.x * c.y };

    return { colA, colB, colC };
}

//==============================================================================

/// Create a left-handed view matrix from two points and a direction.
template <class T>
inline Matrix44<T> look_at_LH(Vector3<T> eye, Vector3<T> target, Vector3<T> up)
{
    Vector f = maths::normalize(target - eye);
    Vector s = maths::normalize(maths::cross(up, f));
    Vector u = maths::cross(f, s);

    Vector colA { s.x, u.x, f.x, T(0.0) };
    Vector colB { s.y, u.y, f.y, T(0.0) };
    Vector colC { s.z, u.z, f.z, T(0.0) };
    Vector colD { -maths::dot(s, eye), -maths::dot(u, eye), -maths::dot(f, eye), T(1.0) };

    return { colA, colB, colC, colD };
}

/// Create a right-handed view matrix from two points and a direction.
template <class T>
inline Matrix44<T> look_at_RH(Vector3<T> eye, Vector3<T> target, Vector3<T> up)
{
    Vector f = maths::normalize(target - eye);
    Vector s = maths::normalize(maths::cross(f, up));
    Vector u = maths::cross(s, f);

    Vector colA { s.x, u.x, -f.x, T(0.0) };
    Vector colB { s.y, u.y, -f.y, T(0.0) };
    Vector colC { s.z, u.z, -f.z, T(0.0) };
    Vector colD { -maths::dot(s, eye), -maths::dot(u, eye), maths::dot(f, eye), T(1.0) };

    return { colA, colB, colC, colD };
}

//==============================================================================

#undef near // mingw or win32 crap
#undef far // mingw or win32 crap

/// Create a left-handed perspective projection matrix.
template <class T>
inline Matrix44<T> perspective_LH(T fov, T aspect, T near, T far)
{
    T tanHalfFov = std::tan(fov * T(0.5));

    Vector colA { T(1.0) / (aspect * tanHalfFov), T(0.0), T(0.0), T(0.0) };
    Vector colB { T(0.0), T(1.0) / tanHalfFov, T(0.0), T(0.0) };
    Vector colC { T(0.0), T(0.0), far / (far - near), T(+1.0) };
    Vector colD { T(0.0), T(0.0), -(far * near) / (far - near), T(0.0) };

    return { colA, colB, colC, colD };
}

/// Create a right-handed perspective projection matrix.
template <class T>
inline Matrix44<T> perspective_RH(T fov, T aspect, T near, T far)
{
    T tanHalfFov = std::tan(fov * T(0.5));

    Vector colA { T(1.0) / (aspect * tanHalfFov), T(0.0), T(0.0), T(0.0) };
    Vector colB { T(0.0), T(1.0) / tanHalfFov, T(0.0), T(0.0) };
    Vector colC { T(0.0), T(0.0), far / (near - far), T(-1.0) };
    Vector colD { T(0.0), T(0.0), -(far * near) / (far - near), T(0.0) };

    return { colA, colB, colC, colD };
}

/// Create a left-handed perspective projection matrix.
template <class T>
inline Matrix44<T> perspective_LH(T fov, T aspect, Vector2<T> range)
{
    return maths::perspective_LH(fov, aspect, range.x, range.y);
}

/// Create a right-handed perspective projection matrix.
template <class T>
inline Matrix44<T> perspective_RH(T fov, T aspect, Vector2<T> range)
{
    return maths::perspective_RH(fov, aspect, range.x, range.y);
}

//==============================================================================

/// Create a left-handed orthographic projection matrix.
template <class T>
inline Matrix44<T> ortho_LH(T l, T r, T b, T t, T n, T f)
{
    Vector colA { T(2.0) / (r-l), T(0.0), T(0.0), T(0.0) };
    Vector colB { T(0.0), T(2.0) / (t-b), T(0.0), T(0.0) };
    Vector colC { T(0.0), T(0.0), T(1.0) / (f-n), T(0.0) };
    Vector colD { -(r+l) / (r-l), -(t+b) / (t-b), -n / (f-n), T(1.0) };

    return { colA, colB, colC, colD };
}

/// Create a right-handed orthographic projection matrix.
template <class T> inline
Matrix44<T> ortho_RH(T l, T r, T b, T t, T n, T f)
{
    // todo: needs updating for vulkan conventions
    T ax = T(2.0) / (r-l); T by = T(2.0) / (t-b); T cz = T(-2.0) / (f-n);
    Vector4<T> colD { -(r+l) / (r-l), -(t+b) / (t-b), -(f+n) / (f-n), 1.0 };
    return Matrix44<T> ( {ax, 0, 0, 0}, {0, by, 0, 0}, {0, 0, cz, 0}, colD );
}

/// Create a left-handed orthographic projection matrix.
template <class T>
inline Matrix44<T> ortho_LH(Vector3<T> min, Vector3<T> max)
{
    return maths::ortho_LH(min.x, max.x, min.y, max.y, min.z, max.z);
}

/// Create a right-handed orthographic projection matrix.
template <class T>
inline Matrix44<T> ortho_RH(Vector3<T> min, Vector3<T> max)
{
    return maths::ortho_RH(min.x, max.x, min.y, max.y, min.z, max.z);
}

//==============================================================================

/// Create an orthonormal basis matrix given the y axis.
template <class T>
inline Matrix33<T> basis_from_y(Vector3<T> yAxis)
{
    Vector tanX = maths::cross(yAxis, Vector3<T>(1, 0, 0));
    Vector tanY = maths::cross(yAxis, Vector3<T>(0, 1, 0));
    Vector tanZ = maths::cross(yAxis, Vector3<T>(0, 0, 1));

    float lenX = maths::length_squared(tanX);
    float lenY = maths::length_squared(tanY);
    float lenZ = maths::length_squared(tanZ);

    Vector3<T> zAxis;
    if (lenX >= lenY && lenX >= lenZ) zAxis = maths::normalize(tanX);
    else if (lenY >= lenZ) zAxis = maths::normalize(tanY);
    else zAxis = maths::normalize(tanZ);

    Vector3<T> xAxis = maths::cross(yAxis, zAxis);

    return { xAxis, yAxis, zAxis };
}

/// Create an orthonormal basis matrix given the z axis.
template <class T>
inline Matrix33<T> basis_from_z(Vector3<T> zAxis)
{
    Vector tanX = maths::cross(zAxis, Vector3<T>(1, 0, 0));
    Vector tanY = maths::cross(zAxis, Vector3<T>(0, 1, 0));
    Vector tanZ = maths::cross(zAxis, Vector3<T>(0, 0, 1));

    float lenX = maths::length_squared(tanX);
    float lenY = maths::length_squared(tanY);
    float lenZ = maths::length_squared(tanZ);

    Vector3<T> yAxis;
    if (lenX >= lenY && lenX >= lenZ) yAxis = maths::normalize(tanX);
    else if (lenY >= lenZ) yAxis = maths::normalize(tanY);
    else yAxis = maths::normalize(tanZ);

    Vector3<T> xAxis = maths::cross(zAxis, yAxis);

    return { xAxis, yAxis, zAxis };
}

} // namespace sq::maths #######################################################
