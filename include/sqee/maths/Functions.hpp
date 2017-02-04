#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq { namespace maths {

//============================================================================//

// Scale (Matrix, Vector3) /////

template <class T> inline
Matrix33<T> scale(Matrix33<T> m, Vector3<T> v)
{
    return Matrix33<T> ( m[0]*v.x, m[1]*v.y, m[2]*v.z );
}

template <class T> inline
Matrix44<T> scale(Matrix44<T> m, Vector3<T> v)
{
    return Matrix44<T> ( m[0]*v.x, m[1]*v.y, m[2]*v.z, m[3] );
}

//============================================================================//

// Translate (Matrix44, Vector3) /////

template <class T> inline
Matrix44<T> translate(Matrix44<T> m, Vector3<T> v)
{
    auto colD = m[0]*v.x + m[1]*v.y + m[2]*v.z + m[3];
    return Matrix44<T> ( m[0], m[1], m[2], colD );
}

//============================================================================//

// Create Transform Model Matrix /////

template <class T> inline
Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, Vector3<T> scale)
{
    auto result = Matrix44<T>(maths::scale(Matrix33<T>(rotation), scale));
    result[3] = Vector4<T> { translation, T(1.0) };

    return result;
}

//============================================================================//

// Create Look At View Matrix /////

template <class T> inline
Matrix44<T> look_at(Vector3<T> eye, Vector3<T> centre, Vector3<T> up)
{
    Vector3<T> f = normalize(centre - eye);
    Vector3<T> s = normalize(maths::cross(f, up));
    Vector3<T> u = maths::cross(s, f);

    Vector4<T> colA { s.x, u.x, -f.x, 0.0 };
    Vector4<T> colB { s.y, u.y, -f.y, 0.0 };
    Vector4<T> colC { s.z, u.z, -f.z, 0.0 };

    Vector4<T> colD { -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

//============================================================================//

// Create Perspective Projection Matrix /////

#undef near // mingw or win32 crap
#undef far // mingw or win32 crap

template <class T> inline
Matrix44<T> perspective(T fov, T aspect, T near, T far)
{
    T tanHalfFov = std::tan(fov * T(0.5));
    T invRange = T(1.0) / (far - near);

    Vector4<T> colA { T(1.0) / (aspect * tanHalfFov), 0.0, 0.0, 0.0 };
    Vector4<T> colB { 0.0, T(1.0) / tanHalfFov, 0.0, 0.0 };
    Vector4<T> colC { 0.0, 0.0, -invRange * (far + near), -1.0 };
    Vector4<T> colD { 0.0, 0.0, -invRange * (T(2.0) * far * near), 0.0 };

    return Matrix44<T> ( colA, colB, colC, colD );
}

template <class T> inline
Matrix44<T> perspective(T fov, T aspect, Vector2<T> range)
{
    return maths::perspective(fov, aspect, range.x, range.y);
}

//============================================================================//

// Create Orthographic Projection Matrix /////

template <class T> inline
Matrix44<T> ortho(T l, T r, T b, T t, T n, T f)
{
    T ax = T(2.0) / (r-l); T by = T(2.0) / (t-b); T cz = T(-2.0) / (f-n);
    Vector4<T> colD { -(r+l) / (r-l), -(t+b) / (t-b), -(f+n) / (f-n), 1.0 };
    return Matrix44<T> ( {ax, 0, 0, 0}, {0, by, 0, 0}, {0, 0, cz, 0}, colD );
}

template <class T> inline
Matrix44<T> ortho(Vector3<T> min, Vector3<T> max)
{
    return maths::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
}

//============================================================================//

}} // namespace sq::maths
