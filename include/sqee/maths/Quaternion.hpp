#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

//============================================================================//

template <class T> struct Quaternion
{
    static_assert(std::is_floating_point_v<T>);

    //--------------------------------------------------------//

    constexpr Quaternion() : data { T(0.0), T(0.0), T(0.0), T(1.0) } {}
    constexpr Quaternion(T x, T y, T z, T w) : data { x, y, z, w } {}
    constexpr Quaternion(const Quaternion& q) : data { q.x, q.y, q.z, q.w } {}

    //--------------------------------------------------------//

    inline explicit Quaternion(T rx, T ry, T rz)
    {
        Vector3<T> halfrads = Vector3<T>(rx, ry, rz);
        halfrads = maths::radians(halfrads) * T(0.5);

        Vector3<T> s = maths::sin(halfrads);
        Vector3<T> c = maths::cos(halfrads);

        x = (s.x * c.y * c.z) - (c.x * s.y * s.z);
        y = (c.x * s.y * c.z) + (s.x * c.y * s.z);
        z = (c.x * c.y * s.z) - (s.x * s.y * c.z);
        w = (c.x * c.y * c.z) + (s.x * s.y * s.z);
    }

    //--------------------------------------------------------//

    inline explicit Quaternion(const Matrix33<T>& m)
    {
        T biggest = m[0][0] + m[1][1] + m[2][2]; int index = 0;
        if (T x = m[0][0] - m[1][1] - m[2][2] > biggest) { biggest = x; index = 1; } // fixme
        if (T y = m[1][1] - m[0][0] - m[2][2] > biggest) { biggest = y; index = 2; } // fixme
        if (T z = m[2][2] - m[0][0] - m[1][1] > biggest) { biggest = z; index = 3; } // fixme
        biggest = std::sqrt(biggest + T(1.0)) * T(0.5);

        T valueA = (T(0.25) / biggest) * (m[0][1] - m[1][0]);
        T valueB = (T(0.25) / biggest) * (m[1][2] - m[2][1]);
        T valueC = (T(0.25) / biggest) * (m[2][0] - m[0][2]);

        if (index == 0) { x = valueB;  y = valueC;  z = valueA;  w = biggest; }
        if (index == 1) { x = biggest; y = valueA;  z = valueC;  w = valueB;  }
        if (index == 2) { x = valueA;  y = biggest; z = valueB;  w = valueC;  }
        if (index == 3) { x = valueC;  y = valueB;  z = biggest; w = valueA;  }
    }

    //--------------------------------------------------------//

    constexpr explicit operator Matrix33<T>() const
    {
        Vector3<T> colA { T(1.0) - T(2.0) * (y*y + z*z), T(2.0) * (x*y + w*z), T(2.0) * (x*z - w*y) };
        Vector3<T> colB { T(2.0) * (x*y - w*z), T(1.0) - T(2.0) * (x*x + z*z), T(2.0) * (y*z + w*x) };
        Vector3<T> colC { T(2.0) * (x*z + w*y), T(2.0) * (y*z - w*x), T(1.0) - T(2.0) * (x*x + y*y) };
        return Matrix33<T> ( colA, colB, colC );
    }

    //--------------------------------------------------------//

    inline explicit Quaternion(const Matrix44<T>& m) : Quaternion(Matrix33<T>(m)) {}

    constexpr explicit operator Matrix44<T>() const { return Matrix44<T> ( Matrix33<T>(*this) ); }

    //--------------------------------------------------------//

    union { T data[4]; struct { T x, y, z, w; }; };
};

//============================================================================//

// multiplication & division (Quaternion, Scalar) /////

template <class T> constexpr
Quaternion<T> operator*(Quaternion<T> q, T s)
{ return { q.x*s, q.y*s, q.z*s, q.w*s }; }

template <class T> constexpr
Quaternion<T> operator/(Quaternion<T> q, T s)
{ return { q.x/s, q.y/s, q.z/s, q.w/s }; }

//============================================================================//

// multiplication (Quaternion, Quaternion) /////

template <class T> constexpr
Quaternion<T> operator*(Quaternion<T> a, Quaternion<T> b)
{
    T x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
    T y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
    T z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
    T w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
    return Quaternion<T> { x, y, z, w };
}

//============================================================================//

// multiplication (Quaternion, Vector3) /////

template <class T> constexpr
Vector3<T> operator*(Quaternion<T> q, Vector3<T> v)
{
    auto qv = maths::cross(Vector3<T>(q.x, q.y, q.z), v);
    auto qqv = maths::cross(Vector3<T>(q.x, q.y, q.z), qv);
    return v + (qv * q.w + qqv) * T(2.0);
}

//============================================================================//

// unary operators (Quaternion) /////

template <class T> constexpr
Quaternion<T> operator+(Quaternion<T> q)
{ return { +q.x, +q.y, +q.z, +q.w }; }

template <class T> constexpr
Quaternion<T> operator-(Quaternion<T> q)
{ return { -q.x, -q.y, -q.z, -q.w }; }

//============================================================================//

namespace maths {

//============================================================================//

// dot product (Quaternion, Quaternion) /////

template <class T> constexpr
T dot(Quaternion<T> a, Quaternion<T> b)
{
    Vector4<T> vecA { a.x, a.y, a.z, a.w };
    Vector4<T> vecB { b.x, b.y, b.z, b.w };
    return maths::dot(vecA, vecB);
}

//============================================================================//

// conjugate (Quaternion) /////

template <class T> constexpr
Quaternion<T> conjugate(Quaternion<T> quat)
{
    return Quaternion<T> { -quat.x, -quat.y, -quat.z, quat.w };
}

//============================================================================//

// inverse (Quaternion) /////

template <class T> constexpr
Quaternion<T> inverse(Quaternion<T> quat)
{
    return maths::conjugate(quat) / maths::dot(quat, quat);
}

//============================================================================//

// length (Quaternion) /////

template <class T> inline
T length(Quaternion<T> quat)
{
    return std::sqrt(maths::dot(quat, quat));
}

//============================================================================//

// normalize (Quaternion) /////

template <class T> inline
Quaternion<T> normalize(Quaternion<T> quat)
{
    return quat * (T(1.0) / maths::length(quat));
}

//============================================================================//

// linear interpolation (Quaternions, Scalar) /////

template <class T> inline
Quaternion<T> lerp(Quaternion<T> a, Quaternion<T> b, T factor)
{
    T invFactor = T(1.0) - factor;
    T x = (a.x * invFactor) + (b.x * factor);
    T y = (a.y * invFactor) + (b.y * factor);
    T z = (a.z * invFactor) + (b.z * factor);
    T w = (a.w * invFactor) + (b.w * factor);
    return Quaternion<T> { x, y, z, w };
}

//============================================================================//

// spherical interpolation (Quaternions, Scalar) /////

template <class T> inline
Quaternion<T> slerp(Quaternion<T> a, Quaternion<T> b, T factor)
{
    T cosine = maths::dot(a, b);
    T absCosine = std::abs(cosine);

    // lerp to prevent division by zero
    if (absCosine > T(1.0) - std::numeric_limits<T>::epsilon())
        return maths::normalize(maths::lerp(a, b, factor));

    // don't go the long way around
    if (cosine < T(0.0)) b = -b;

    T angle = std::acos(absCosine), sine = std::sin(angle);
    T factorA = std::sin(angle * (T(1.0) - factor)) / sine;
    T factorB = std::sin(angle * factor) / sine;

    T x = (a.x * factorA) + (b.x * factorB);
    T y = (a.y * factorA) + (b.y * factorB);
    T z = (a.z * factorA) + (b.z * factorB);
    T w = (a.w * factorA) + (b.w * factorB);
    return Quaternion<T> { x, y, z, w };
}

//============================================================================//

}} // namespace sq::maths

using QuatF = sq::Quaternion<float>;
