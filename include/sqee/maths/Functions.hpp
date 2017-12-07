#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq::maths {

//============================================================================//

/// Scale a Matrix33 by Vector3.
template <class T> constexpr
Matrix33<T> scale(Matrix33<T> m, Vector3<T> v)
{
    return Matrix33<T> ( m[0]*v.x, m[1]*v.y, m[2]*v.z );
}

/// Scale a Matrix44 by Vector3.
template <class T> constexpr
Matrix44<T> scale(Matrix44<T> m, Vector3<T> v)
{
    return Matrix44<T> ( m[0]*v.x, m[1]*v.y, m[2]*v.z, m[3] );
}

//============================================================================//

/// Translate a Matrix44 by Vector3.
template <class T> constexpr
Matrix44<T> translate(Matrix44<T> m, Vector3<T> v)
{
    auto colD = m[0]*v.x + m[1]*v.y + m[2]*v.z + m[3];
    return Matrix44<T> ( m[0], m[1], m[2], colD );
}

//============================================================================//

/// Create a matrix from a translation, rotation, and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation, Vector3<T> scale)
{
    auto result = Matrix44<T> ( maths::scale(rotation, scale) );
    result[3] = Vector4<T> ( translation, T(1.0) );

    return result;
}

/// Create a matrix from a translation, rotation, and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation, T scale)
{
    return maths::transform(translation, rotation, Vec3F(scale));
}

/// Create a matrix from a translation, rotation, and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, Vector3<T> scale)
{
    return maths::transform(translation, Matrix33<T>(rotation), scale);
}

/// Create a matrix from a translation, rotation, and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, T scale)
{
    return maths::transform(translation, Matrix33<T>(rotation), Vec3F(scale));
}

//============================================================================//

/// Create a matrix from a translation and rotation.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Matrix33<T> rotation)
{
    auto result = Matrix44<T> ( rotation );
    result[3] = Vector4<T> ( translation, T(1.0) );

    return result;
}

/// Create a matrix from a translation and rotation.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation)
{
    return maths::transform(translation, Matrix33<T>(rotation));
}

//============================================================================//

/// Create a matrix from a translation and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, Vector3<T> scale)
{
    Vector4<T> colA { scale.x, 0.0, 0.0, 0.0 };
    Vector4<T> colB { 0.0, scale.y, 0.0, 0.0 };
    Vector4<T> colC { 0.0, 0.0, scale.z, 0.0 };
    Vector4<T> colD ( translation, T(1.0) );

    return Matrix44<T> ( colA, colB, colC, colD );
}

/// Create a matrix from a translation and scale.
template <class T> constexpr
Matrix44<T> transform(Vector3<T> translation, T scale)
{
    return maths::transform(translation, Vec3F(scale));
}

//============================================================================//

/// Create a rotation matrix from an axis and angle.
template <class T> constexpr
Matrix33<T> rotation(Vector3<T> axis, T angle)
{
    const T c = std::cos(maths::radians(angle)), s = std::sin(maths::radians(angle));
    const Vector3<T> x = axis * axis.x, y = axis * axis.y, z = axis * axis.z;

    Vector3<T> colA { x.x + (T(1)-x.x) * c, x.y * (T(1)-c) - axis.z * s, x.z * (T(1)-c) + axis.y * s };
    Vector3<T> colB { y.x * (T(1)-c) + axis.z * s, y.y + (T(1)-y.y) * c, y.z * (T(1)-c) - axis.x * s };
    Vector3<T> colC { z.x * (T(1)-c) - axis.y * s, z.y * (T(1)-c) + axis.x * s, z.z + (T(1)-z.z) * c };

    return Matrix33<T> { colA, colB, colC };
}

//============================================================================//

/// Create a left-handed view matrix from two points and a direction.
template <class T> inline
Matrix44<T> look_at_LH(Vector3<T> eye, Vector3<T> target, Vector3<T> up)
{
    Vector3<T> f = normalize(target - eye);
    Vector3<T> s = normalize(maths::cross(up, f));
    Vector3<T> u = maths::cross(f, s);

    Vector4<T> colA { s.x, u.x, f.x, 0.0 };
    Vector4<T> colB { s.y, u.y, f.y, 0.0 };
    Vector4<T> colC { s.z, u.z, f.z, 0.0 };

    Vector4<T> colD { -dot(s, eye), -dot(u, eye), -dot(f, eye), 1.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

/// Create a right-handed view matrix from two points and a direction.
template <class T> inline
Matrix44<T> look_at_RH(Vector3<T> eye, Vector3<T> target, Vector3<T> up)
{
    Vector3<T> f = normalize(target - eye);
    Vector3<T> s = normalize(maths::cross(f, up));
    Vector3<T> u = maths::cross(s, f);

    Vector4<T> colA { s.x, u.x, -f.x, 0.0 };
    Vector4<T> colB { s.y, u.y, -f.y, 0.0 };
    Vector4<T> colC { s.z, u.z, -f.z, 0.0 };

    Vector4<T> colD { -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

//============================================================================//

#undef near // mingw or win32 crap
#undef far // mingw or win32 crap

/// Create a left-handed perspective projection matrix.
template <class T> inline
Matrix44<T> perspective_LH(T fov, T aspect, T near, T far)
{
    T tanHalfFov = std::tan(fov * T(0.5));
    T invRange = T(1.0) / (far - near);

    Vector4<T> colA { T(1.0) / (aspect * tanHalfFov), 0.0, 0.0, 0.0 };
    Vector4<T> colB { 0.0, T(1.0) / tanHalfFov, 0.0, 0.0 };
    Vector4<T> colC { 0.0, 0.0, invRange * +(far + near), +1.0 };
    Vector4<T> colD { 0.0, 0.0, invRange * -(T(2.0) * far * near), 0.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

/// Create a right-handed perspective projection matrix.
template <class T> inline
Matrix44<T> perspective_RH(T fov, T aspect, T near, T far)
{
    T tanHalfFov = std::tan(fov * T(0.5));
    T invRange = T(1.0) / (far - near);

    Vector4<T> colA { T(1.0) / (aspect * tanHalfFov), 0.0, 0.0, 0.0 };
    Vector4<T> colB { 0.0, T(1.0) / tanHalfFov, 0.0, 0.0 };
    Vector4<T> colC { 0.0, 0.0, invRange * -(far + near), -1.0 };
    Vector4<T> colD { 0.0, 0.0, invRange * -(T(2.0) * far * near), 0.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

/// Create a left-handed perspective projection matrix.
template <class T> inline
Matrix44<T> perspective_LH(T fov, T aspect, Vector2<T> range)
{
    return maths::perspective_LH(fov, aspect, range.x, range.y);
}

/// Create a right-handed perspective projection matrix.
template <class T> inline
Matrix44<T> perspective_RH(T fov, T aspect, Vector2<T> range)
{
    return maths::perspective_RH(fov, aspect, range.x, range.y);
}

//============================================================================//

/// Create a left-handed orthographic projection matrix.
template <class T> inline
Matrix44<T> ortho_LH(T l, T r, T b, T t, T n, T f)
{
    T ax = T(2.0) / (r-l); T by = T(2.0) / (t-b);  T cz = T(+2.0) / (f-n);
    Vector4<T> colD { -(r+l) / (r-l), -(t+b) / (t-b), -(f+n) / (f-n), 1.0 };
    return Matrix44<T> ( {ax, 0, 0, 0}, {0, by, 0, 0}, {0, 0, cz, 0}, colD );
}

/// Create a right-handed orthographic projection matrix.
template <class T> inline
Matrix44<T> ortho_RH(T l, T r, T b, T t, T n, T f)
{
    T ax = T(2.0) / (r-l); T by = T(2.0) / (t-b); T cz = T(-2.0) / (f-n);
    Vector4<T> colD { -(r+l) / (r-l), -(t+b) / (t-b), -(f+n) / (f-n), 1.0 };
    return Matrix44<T> ( {ax, 0, 0, 0}, {0, by, 0, 0}, {0, 0, cz, 0}, colD );
}

/// Create a left-handed orthographic projection matrix.
template <class T> inline
Matrix44<T> ortho_LH(Vector3<T> min, Vector3<T> max)
{
    return maths::ortho_LH(min.x, max.x, min.y, max.y, min.z, max.z);
}

/// Create a right-handed orthographic projection matrix.
template <class T> inline
Matrix44<T> ortho_RH(Vector3<T> min, Vector3<T> max)
{
    return maths::ortho_RH(min.x, max.x, min.y, max.y, min.z, max.z);
}

//============================================================================//

/// Create an orthonormal basis matrix given the y axis.
template <class T> inline
Matrix33<T> basis_from_y(Vector3<T> yAxis)
{
    Vec3F zAxis;

    const Vec3F tanX = maths::cross(yAxis, Vec3F(1.f, 0.f, 0.f));
    const Vec3F tanY = maths::cross(yAxis, Vec3F(0.f, 1.f, 0.f));
    const Vec3F tanZ = maths::cross(yAxis, Vec3F(0.f, 0.f, 1.f));

    const float lenX = maths::length_squared(tanX);
    const float lenY = maths::length_squared(tanY);
    const float lenZ = maths::length_squared(tanZ);

    if (lenX >= lenY && lenX >= lenZ) zAxis = maths::normalize(tanX);
    else if (lenY >= lenZ) zAxis = maths::normalize(tanY);
    else zAxis = maths::normalize(tanZ);

    const Vec3F xAxis = maths::cross(yAxis, zAxis);

    return Mat3F ( xAxis, yAxis, zAxis );
}

/// Create an orthonormal basis matrix given the z axis.
template <class T> inline
Matrix33<T> basis_from_z(Vector3<T> zAxis)
{
    Vec3F yAxis;

    const Vec3F tanX = maths::cross(zAxis, Vec3F(1.f, 0.f, 0.f));
    const Vec3F tanY = maths::cross(zAxis, Vec3F(0.f, 1.f, 0.f));
    const Vec3F tanZ = maths::cross(zAxis, Vec3F(0.f, 0.f, 1.f));

    const float lenX = maths::length_squared(tanX);
    const float lenY = maths::length_squared(tanY);
    const float lenZ = maths::length_squared(tanZ);

    if (lenX >= lenY && lenX >= lenZ) yAxis = maths::normalize(tanX);
    else if (lenY >= lenZ) yAxis = maths::normalize(tanY);
    else yAxis = maths::normalize(tanZ);

    const Vec3F xAxis = maths::cross(zAxis, yAxis);

    return Mat3F ( xAxis, yAxis, zAxis );
}

//============================================================================//

} // namespace sq::maths
