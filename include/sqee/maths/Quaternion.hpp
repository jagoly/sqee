// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

#include <fmt/format.h>

namespace sq { namespace maths {

//============================================================================//

template <class T> struct Quaternion
{
    static_assert(std::is_floating_point_v<T>);

    // Default and Value Constructor
    constexpr Quaternion() : data { T(0), T(0), T(0), T(1) } {}
    constexpr Quaternion(T x, T y, T z, T w) : data { x, y, z, w } {}

    // Copy Constructor
    constexpr Quaternion(const Quaternion& q) = default;

    // Uninitialise Constructor
    constexpr Quaternion(std::nullptr_t) {}

    /// Construct a quaternion from an XYZ euler rotation.
    inline explicit Quaternion(T rx, T ry, T rz)
    {
        Vector3<T> halfrads = maths::radians(Vector3<T>(rx, ry, rz)) * T(0.5);

        Vector3<T> s = maths::sin(halfrads);
        Vector3<T> c = maths::cos(halfrads);

        x = (s.x * c.y * c.z) - (c.x * s.y * s.z);
        y = (c.x * s.y * c.z) + (s.x * c.y * s.z);
        z = (c.x * c.y * s.z) - (s.x * s.y * c.z);
        w = (c.x * c.y * c.z) + (s.x * s.y * s.z);
    }

    /// Construct a quaternion from a 3x3 rotation matrix.
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

    /// Convert a quaternion to a 3x3 rotation matrix.
    constexpr explicit operator Matrix33<T>() const
    {
        Vector3<T> colA { T(1) - T(2) * (y*y + z*z), T(2) * (x*y + w*z), T(2) * (x*z - w*y) };
        Vector3<T> colB { T(2) * (x*y - w*z), T(1) - T(2) * (x*x + z*z), T(2) * (y*z + w*x) };
        Vector3<T> colC { T(2) * (x*z + w*y), T(2) * (y*z - w*x), T(1) - T(2) * (x*x + y*y) };
        return Matrix33<T> ( colA, colB, colC );
    }

    /// Construct a quaternion from a 4x4 rotation matrix.
    inline explicit Quaternion(const Matrix44<T>& m) : Quaternion(Matrix33<T>(m)) {}

    /// Convert a quaternion to a 4x4 rotation matrix.
    constexpr explicit operator Matrix44<T>() const { return Matrix44<T>(Matrix33<T>(*this)); }

    /// Cast a quaternion to a Vector4.
    constexpr explicit operator Vector4<T>() const { return { x, y, z, w }; }

    // Array Access Operators
    constexpr T& operator[](int index) { return data[index]; }
    constexpr const T& operator[](int index) const { return data[index]; }

    union { T data[4]; struct { T x, y, z, w; }; };
};

//============================================================================//

// equality, inequality (Quaternion, Quaternion) /////

template <class T> constexpr
bool operator==(Quaternion<T> a, Quaternion<T> b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template <class T> constexpr
bool operator!=(Quaternion<T> a, Quaternion<T> b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

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

/// Compute the dot product of two quaternions.
template <class T> constexpr
T dot(Quaternion<T> a, Quaternion<T> b)
{
    Vector4<T> vecA { a.x, a.y, a.z, a.w };
    Vector4<T> vecB { b.x, b.y, b.z, b.w };
    return maths::dot(vecA, vecB);
}

//============================================================================//

/// Compute the conjugate of a quaternion.
template <class T> constexpr
Quaternion<T> conjugate(Quaternion<T> quat)
{
    return Quaternion<T> { -quat.x, -quat.y, -quat.z, quat.w };
}

//============================================================================//

/// Compute the inverse of a quaternion.
template <class T> constexpr
Quaternion<T> inverse(Quaternion<T> quat)
{
    return maths::conjugate(quat) / maths::dot(quat, quat);
}

//============================================================================//

/// Compute the length of a quaternion.
template <class T> inline
T length(Quaternion<T> quat)
{
    return std::sqrt(maths::dot(quat, quat));
}

//============================================================================//

/// Compute the unit length quaternion.
template <class T> inline
Quaternion<T> normalize(Quaternion<T> quat)
{
    return quat * (T(1.0) / maths::length(quat));
}

//============================================================================//

/// Compute the angular difference between two quaternions.
template <class T> inline
T angular_difference(Quaternion<T> a, Quaternion<T> b)
{
    Quaternion rot = maths::inverse(a) * b;
    Vector3<T> vec = { rot.x, rot.y, rot.z };

    return maths::cycles(T(2.0) * std::atan2(maths::length(vec), rot.w));
}

//============================================================================//

/// Linearly interpolate between two quaternions.
template <class T> inline
Quaternion<T> lerp(Quaternion<T> a, Quaternion<T> b, T factor)
{
    T invFactor = T(1.0) - factor;
    T x = (a.x * invFactor) + (b.x * factor);
    T y = (a.y * invFactor) + (b.y * factor);
    T z = (a.z * invFactor) + (b.z * factor);
    T w = (a.w * invFactor) + (b.w * factor);

    // note that this result is not normalised
    return Quaternion<T> { x, y, z, w };
}

//============================================================================//

/// Spherically interpolate between two unit quaternions.
template <class T> inline
Quaternion<T> slerp(Quaternion<T> a, Quaternion<T> b, T factor)
{
    T cosine = maths::dot(a, b);
    T absCosine = std::abs(cosine);

    // don't go the long way around
    if (cosine < T(0.0)) b = -b;

    // lerp to prevent division by zero
    if (T(1.0) - absCosine <= std::numeric_limits<T>::epsilon())
        return maths::normalize(maths::lerp(a, b, factor));

    T angle = std::acos(absCosine);
    T sine = std::sin(angle);
    T factorA = std::sin(angle * (T(1.0) - factor)) / sine;
    T factorB = std::sin(angle * factor) / sine;

    T x = (a.x * factorA) + (b.x * factorB);
    T y = (a.y * factorA) + (b.y * factorB);
    T z = (a.z * factorA) + (b.z * factorB);
    T w = (a.w * factorA) + (b.w * factorB);

    // if a and b were normalised, this will be too
    return Quaternion<T> { x, y, z, w };
}

//============================================================================//

/// Lerp between two quaternions using the shorter path.
template <class T> inline
Quaternion<T> lerp_shortest(Quaternion<T> a, Quaternion<T> b, T factor)
{
    // don't go the long way around
    if (maths::dot(a, b) < T(0.0)) b = -b;

    return maths::normalize(maths::lerp(a, b, factor));
}

//============================================================================//

/// Lerp between two quaternions in whichever direction is closest to the guide.
template <class T> inline
Quaternion<T> lerp_guided(Quaternion<T> a, Quaternion<T> b, T factor, Quaternion<T> guide)
{
    Quaternion<T> resultPos = maths::lerp(a, +b, factor);
    Quaternion<T> resultNeg = maths::lerp(a, -b, factor);

    Quaternion<T> invGuide = maths::inverse(guide);

    Quaternion<T> rotPos = resultPos * invGuide;
    Quaternion<T> rotNeg = resultNeg * invGuide;

    T lengthPos = maths::length(Vector3<T>(rotPos.x, rotPos.y, rotPos.z));
    T lengthNeg = maths::length(Vector3<T>(rotNeg.x, rotNeg.y, rotNeg.z));

    T diffPos = std::abs(std::atan2(lengthPos, rotPos.w));
    T diffNeg = std::abs(std::atan2(lengthNeg, rotNeg.w));

    return maths::normalize(diffPos < diffNeg ? resultPos : resultNeg);
}

//============================================================================//

} // namespace maths

template <class T>
constexpr const char* type_to_string(maths::Quaternion<T>)
{
    if constexpr (std::is_same_v<T, float>) return "QuatF";
    return "Quaternion<T>";
}

//============================================================================//

namespace detail {

template <class T>
struct QuaternionTraits : std::false_type {};

template <class T>
struct QuaternionTraits<const T> : QuaternionTraits<T> {};

template <class T>
struct QuaternionTraits<maths::Quaternion<T>> : std::true_type
{
    static constexpr bool value = true;
    using type = T;
};

template <class T>
constexpr bool is_quaternion_v = QuaternionTraits<T>::value;

template <class T>
using quaternion_type_t = typename QuaternionTraits<T>::type;

}} // namespace sq::detail

//============================================================================//

template <class T>
struct fmt::formatter<sq::maths::Quaternion<T>> : fmt::formatter<T>
{
    template <class FormatContext>
    auto format(const sq::maths::Quaternion<T>& quat, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "{}(", sq::type_to_string(sq::maths::Quaternion<T>()));
        formatter<T>::format(quat.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(quat.y, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(quat.z, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(quat.w, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};
