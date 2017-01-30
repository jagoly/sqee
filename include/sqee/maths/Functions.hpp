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

// Create Transform Matrix /////

template <class T> inline
Matrix44<T> transform(Vector3<T> translation, Quaternion<T> rotation, Vector3<T> scale)
{
    auto mat33 = maths::scale(Matrix33<T>(rotation), scale);
    //mat33 = mat33 * Matrix33<T>(rotation);

    auto result = Matrix44<T>(mat33);
    result[3] = Vector4<T> { translation, T(1.0) };

    return result;
}

//============================================================================//

}} // namespace sq::maths
