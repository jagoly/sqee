// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq::maths { //########################################################

template <class T> struct Quaternion;

/// Concept that matches any kind of quaternion.
template <class QuaternionT>
concept AnyQuaternion = requires (QuaternionT quat)
{
    []<int S, class T>(Quaternion<T>&){}(quat);
};

//==============================================================================

template <class T> struct Quaternion
{
    static_assert(std::is_floating_point_v<T>);

    T x, y, z, w;
\
    constexpr Quaternion() : x(0), y(0), z(0), w(1) {}
    constexpr Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    constexpr Quaternion(const Quaternion& q) = default;
    constexpr Quaternion& operator=(const Quaternion& q) = default;

    /// Construct without initialisation.
    Quaternion(std::nullptr_t) {}

    /// Construct from a quaternion of a different scalar type.
    template <class U>
    constexpr explicit Quaternion(Quaternion<U> q) : Quaternion(T(q.x), T(q.y), T(q.z), T(q.w)) {}

    /// Construct from an XYZ euler rotation.
    inline explicit Quaternion(T rx, T ry, T rz)
    {
        Vector halfRads = maths::radians(Vector(rx, ry, rz)) * T(0.5);

        Vector s = maths::sin(halfRads);
        Vector c = maths::cos(halfRads);

        x = (s.x * c.y * c.z) - (c.x * s.y * s.z);
        y = (c.x * s.y * c.z) + (s.x * c.y * s.z);
        z = (c.x * c.y * s.z) - (s.x * s.y * c.z);
        w = (c.x * c.y * c.z) + (s.x * s.y * s.z);
    }

    /// Construct from a 3x3 rotation matrix.
    inline explicit Quaternion(const Matrix33<T>& m)
    {
        T biggest = m[0][0] + m[1][1] + m[2][2]; int index = 0; // w
        if (T bx = m[0][0] - m[1][1] - m[2][2]; bx > biggest) { biggest = bx; index = 1; }
        if (T by = m[1][1] - m[0][0] - m[2][2]; by > biggest) { biggest = by; index = 2; }
        if (T bz = m[2][2] - m[0][0] - m[1][1]; bz > biggest) { biggest = bz; index = 3; }

        T value = std::sqrt(biggest + T(1.0)) * T(0.5);
        T mul = T(0.25) / value;

        if (index == 0) { x = (m[1][2] - m[2][1]) * mul; y = (m[2][0] - m[0][2]) * mul; z = (m[0][1] - m[1][0]) * mul; w = value; }
        if (index == 1) { x = value; y = (m[0][1] + m[1][0]) * mul; z = (m[2][0] + m[0][2]) * mul; w = (m[1][2] - m[2][1]) * mul; }
        if (index == 2) { x = (m[0][1] + m[1][0]) * mul; y = value; z = (m[1][2] + m[2][1]) * mul; w = (m[2][0] - m[0][2]) * mul; }
        if (index == 3) { x = (m[2][0] + m[0][2]) * mul; y = (m[1][2] + m[2][1]) * mul; z = value; w = (m[0][1] - m[1][0]) * mul; }
    }

    /// Construct from a 4x4 rotation matrix.
    inline explicit Quaternion(const Matrix44<T>& m) : Quaternion(Matrix33<T>(m)) {}

    /// Convert to a 3x3 rotation matrix.
    constexpr explicit operator Matrix33<T>() const
    {
        Vector colA { T(1) - T(2) * (y*y + z*z), T(2) * (x*y + w*z), T(2) * (x*z - w*y) };
        Vector colB { T(2) * (x*y - w*z), T(1) - T(2) * (x*x + z*z), T(2) * (y*z + w*x) };
        Vector colC { T(2) * (x*z + w*y), T(2) * (y*z - w*x), T(1) - T(2) * (x*x + y*y) };

        return { colA, colB, colC };
    }

    /// Convert to a 4x4 rotation matrix.
    constexpr explicit operator Matrix44<T>() const { return Matrix44<T>(Matrix33<T>(*this)); }

    /// Convert to a four component vector.
    constexpr explicit operator Vector4<T>() const { return { x, y, z, w }; }

    constexpr const T& operator[](int index) const
    {
        if (std::is_constant_evaluated())
        {
            if (index == 0) return x;
            if (index == 1) return y;
            if (index == 2) return z;
            if (index == 3) return w;
        }
        return reinterpret_cast<const T*>(this)[index];
    }

    constexpr T& operator[](int index)
    {
        return const_cast<T&>(static_cast<const Quaternion&>(*this)[index]);
    }

    const T* data() const { return reinterpret_cast<const T*>(this); }

    T* data() { return reinterpret_cast<T*>(this); }
};

template <class T> Quaternion(T, T, T, T) -> Quaternion<T>;

//==============================================================================

template <class T>
constexpr bool operator==(Quaternion<T> a, Quaternion<T> b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template <class T>
constexpr Quaternion<T> operator*(Quaternion<T> q, T s)
{
    return { q.x*s, q.y*s, q.z*s, q.w*s };
}

template <class T>
constexpr Quaternion<T> operator/(Quaternion<T> q, T s)
{
    return { q.x/s, q.y/s, q.z/s, q.w/s };
}

template <class T>
constexpr Quaternion<T> operator*(Quaternion<T> a, Quaternion<T> b)
{
    T x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
    T y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
    T z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
    T w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
    return { x, y, z, w };
}

template <class T>
constexpr Vector3<T> operator*(Quaternion<T> q, Vector3<T> v)
{
    Vector qv = maths::cross(Vector(q.x, q.y, q.z), v);
    Vector qqv = maths::cross(Vector(q.x, q.y, q.z), qv);
    return v + (qv * q.w + qqv) * T(2.0);
}

template <class T>
constexpr Quaternion<T> operator+(Quaternion<T> q)
{
    return { +q.x, +q.y, +q.z, +q.w };
}

template <class T>
constexpr Quaternion<T> operator-(Quaternion<T> q)
{
    return { -q.x, -q.y, -q.z, -q.w };
}

//==============================================================================

/// Compute the dot product of two quaternions.
template <class T>
constexpr T dot(Quaternion<T> a, Quaternion<T> b)
{
    Vector vecA { a.x, a.y, a.z, a.w };
    Vector vecB { b.x, b.y, b.z, b.w };
    return maths::dot(vecA, vecB);
}

/// Compute the conjugate of a quaternion.
template <class T>
constexpr Quaternion<T> conjugate(Quaternion<T> quat)
{
    return { -quat.x, -quat.y, -quat.z, quat.w };
}

/// Compute the inverse of a quaternion.
template <class T>
constexpr Quaternion<T> inverse(Quaternion<T> quat)
{
    return maths::conjugate(quat) / maths::dot(quat, quat);
}

/// Compute the length of a quaternion.
template <class T>
inline T length(Quaternion<T> quat)
{
    return std::sqrt(maths::dot(quat, quat));
}

/// Scale a quaternion to a length of one.
template <class T>
inline Quaternion<T> normalize(Quaternion<T> quat)
{
    return quat * (T(1.0) / maths::length(quat));
}

/// Compute the angular difference between two quaternions.
template <class T>
inline T angular_difference(Quaternion<T> a, Quaternion<T> b)
{
    Quaternion rot = maths::inverse(a) * b;
    T halfRads = std::atan2(maths::length(Vector(rot.x, rot.y, rot.z)), rot.w);
    return maths::cycles(T(2.0) * halfRads);
}

//==============================================================================

/// Linearly interpolate between two quaternions.
template <class T>
inline Quaternion<T> lerp(Quaternion<T> a, Quaternion<T> b, T factor)
{
    T invFactor = T(1.0) - factor;
    T x = (a.x * invFactor) + (b.x * factor);
    T y = (a.y * invFactor) + (b.y * factor);
    T z = (a.z * invFactor) + (b.z * factor);
    T w = (a.w * invFactor) + (b.w * factor);

    // note that this is not normalised
    return { x, y, z, w };
}

/// Spherically interpolate between two unit quaternions.
template <class T>
inline Quaternion<T> slerp(Quaternion<T> a, Quaternion<T> b, T factor)
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
    return { x, y, z, w };
}

/// Lerp between two quaternions using the shorter path.
template <class T>
inline Quaternion<T> lerp_shorter(Quaternion<T> a, Quaternion<T> b, T factor)
{
    // don't go the long way around
    if (maths::dot(a, b) < T(0.0)) b = -b;

    return maths::normalize(maths::lerp(a, b, factor));
}

/// Lerp between two quaternions in whichever direction is closest to the guide.
template <class T>
inline Quaternion<T> lerp_guided(Quaternion<T> a, Quaternion<T> b, T factor, Quaternion<T> guide)
{
    Quaternion resultPos = maths::lerp(a, +b, factor);
    Quaternion resultNeg = maths::lerp(a, -b, factor);

    Quaternion invGuide = maths::inverse(guide);

    Quaternion rotPos = resultPos * invGuide;
    Quaternion rotNeg = resultNeg * invGuide;

    T lengthPos = maths::length(Vector(rotPos.x, rotPos.y, rotPos.z));
    T lengthNeg = maths::length(Vector(rotNeg.x, rotNeg.y, rotNeg.z));

    T diffPos = std::abs(std::atan2(lengthPos, rotPos.w));
    T diffNeg = std::abs(std::atan2(lengthNeg, rotNeg.w));

    return maths::normalize(diffPos < diffNeg ? resultPos : resultNeg);
}

} // namespace sq::maths #######################################################

#include <sqee/core/TypeNames.hpp>

#include <fmt/format.h>

template <class T>
struct fmt::formatter<sq::maths::Quaternion<T>>
{
    fmt::formatter<T> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    FormatContext::iterator format(const sq::maths::Quaternion<T>& quat, FormatContext& ctx) const
    {
        ctx.advance_to(fmt::detail::write(ctx.out(), sq::type_name_v<sq::maths::Quaternion<T>>.c_str()));
        ctx.advance_to(fmt::detail::write(ctx.out(), '('));
        ctx.advance_to(base.format(quat.x, ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(quat.y, ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(quat.z, ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(quat.w, ctx));
        return fmt::detail::write(ctx.out(), ')');
    }
};
