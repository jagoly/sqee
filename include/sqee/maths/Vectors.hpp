#pragma once

#include <sqee/maths/Scalar.hpp>

namespace sq {

//============================================================================//

template <int S, class T> struct Vector
{
    static_assert(std::is_arithmetic_v<T>);
};

template <class T> using Vector2 = Vector<2, T>;
template <class T> using Vector3 = Vector<3, T>;
template <class T> using Vector4 = Vector<4, T>;

//============================================================================//

template <class T> struct Vector<2, T>
{
    constexpr Vector() : data { T(0), T(0) } {}
    constexpr Vector(T x, T y) : data { x, y } {}

    constexpr explicit Vector(T s) : Vector(s, s) {}

    // Copy Constructors
    constexpr Vector(const Vector<2, T>& v) = default;
    template<class U> explicit Vector(Vector2<U> v) : Vector(T(v.x), T(v.y)) {}

    // Swizzle Constructors
    constexpr explicit Vector(Vector3<T> v) : Vector(v.x, v.y) {}
    constexpr explicit Vector(Vector4<T> v) : Vector(v.x, v.y) {}

    // Array Access Operators
    constexpr T& operator[](int index) { return data[index]; }
    constexpr const T& operator[](int index) const { return data[index]; }

    union { T data[2]; struct { T x, y; }; struct { T r, g; }; };
};

//============================================================================//

template <class T> struct Vector<3, T>
{
    constexpr Vector() : data { T(0), T(0), T(0) } {}
    constexpr Vector(T x, T y, T z) : data { x, y, z } {}

    constexpr explicit Vector(T s) : Vector(s, s, s) {}

    // Copy Constructors
    constexpr Vector(const Vector<3, T>& v) = default;
    template<class U> explicit Vector(Vector3<U> v) : Vector(T(v.x), T(v.y), T(v.z)) {}

    // Swizzle Constructors
    constexpr explicit Vector(Vector4<T> v) : Vector(v.x, v.y, v.z) {}
    constexpr explicit Vector(Vector2<T> xy, T z) : Vector(xy.x, xy.y, z) {}
    constexpr explicit Vector(T x, Vector2<T> yz) : Vector(x, yz.x, yz.y) {}

    // Array Access Operators
    constexpr T& operator[](int index) { return data[index]; }
    constexpr const T& operator[](int index) const { return data[index]; }

    union { T data[3]; struct { T x, y, z; }; struct { T r, g, b; }; };
};

//============================================================================//

template <class T> struct Vector<4, T>
{
    constexpr Vector() : data { T(0), T(0), T(0), T(0) } {}
    constexpr Vector(T x, T y, T z, T w) : data { x, y, z, w } {}

    constexpr explicit Vector(T s) : Vector(s, s, s, s) {}

    // Copy Constructors
    constexpr Vector(const Vector<4, T>& v) = default;
    template<class U> explicit Vector(Vector4<U> v) : Vector(T(v.x), T(v.y), T(v.z), T(v.w)) {}

    // Swizzle Constructors
    constexpr explicit Vector(Vector2<T> xy, T z, T w) : Vector(xy.x, xy.y, z, w) {}
    constexpr explicit Vector(T x, Vector2<T> yz, T w) : Vector(x, yz.x, yz.y, w) {}
    constexpr explicit Vector(T x, T y, Vector2<T> zw) : Vector(x, y, zw.x, zw.y) {}
    constexpr explicit Vector(Vector3<T> xyz, T w) : Vector(xyz.x, xyz.y, xyz.z, w) {}
    constexpr explicit Vector(T x, Vector3<T> yzw) : Vector(x, yzw.x, yzw.y, yzw.z) {}

    // Array Access Operators
    constexpr T& operator[](int index) { return data[index]; }
    constexpr const T& operator[](int index) const { return data[index]; }

    union { T data[4]; struct { T x, y, z, w; }; struct { T r, g, b, a; }; };
};

//============================================================================//

// equality, inequality (Vector, Vector) /////

template <int S, class T> constexpr
bool operator==(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        if (a[i] != b[i]) return false;

    return true;
}

template <int S, class T> constexpr
bool operator!=(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        if (a[i] != b[i]) return true;

    return false;
}

//============================================================================//

// integral comparison (Vector, Vector) /////

template <int S, class T> constexpr
bool operator<(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_integral_v<T>);

    for (int i = 0; i < S; ++i)
        if (a[i] != b[i]) return a[i] < b[i];

    return false;
}

//============================================================================//

// arithmetic binary operators (Vector, Vector) /////

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator+(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] += b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator+=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] += b[i]; return a; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] -= b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator-=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] -= b[i]; return a; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator*(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] *= b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator*=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] *= b[i]; return a; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator/(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] /= b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator/=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] /= b[i]; return a; }

//============================================================================//

// arithmetic binary operators (Vector, Scalar) /////

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator+(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] += s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator+=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] += s; return v; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] -= s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator-=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] -= s; return v; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator*(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] *= s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator*=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] *= s; return v; }

//--------------------------------------------------------//

template <int S, class T> constexpr
Vector<S, T> operator/(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] /= s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator/=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] /= s; return v; }

//============================================================================//

// arithmetic unary operators (Vector) /////

template <int S, class T> constexpr
Vector<S, T> operator+(Vector<S, T> vec)
{
    static_assert(std::is_signed_v<T>);

    return Vector<S, T>() + vec;
}

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> vec)
{
    static_assert(std::is_signed_v<T>);

    return Vector<S, T>() - vec;
}

//============================================================================//

namespace maths {

//============================================================================//

/// Component-wise a is less than b test.
template <int S, class T> constexpr
bool all_less(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) if (a[i] >= b[i]) return false;
    return true;
}

/// Component-wise a is greater than b test.
template <int S, class T> constexpr
bool all_greater(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) if (a[i] <= b[i]) return false;
    return true;
}

/// Component-wise a is less than or equal to b test.
template <int S, class T> constexpr
bool all_less_equal(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) if (a[i] > b[i]) return false;
    return true;
}

/// Component-wise a is greater than or equal to b test.
template <int S, class T> constexpr
bool all_greater_equal(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) if (a[i] < b[i]) return false;
    return true;
}

//============================================================================//

/// Compute the component-wise minimum of two vectors.
template <int S, class T> constexpr
Vector<S, T> min(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        a[i] = maths::min(a[i], b[i]);

    return a;
}

/// Compute the component-wise maximum of two vectors.
template <int S, class T> constexpr
Vector<S, T> max(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        a[i] = maths::max(a[i], b[i]);

    return a;
}

//============================================================================//

/// Clamp a vector to within the specified range.
template <int S, class T> constexpr
Vector<S, T> clamp(Vector<S, T> vec, Vector<S, T> min, Vector<S, T> max)
{
    return maths::min(maths::max(vec, min), max);
}

//============================================================================//

/// Linearly interpolate between two vectors.
template <int S, class T> constexpr
Vector<S, T> mix(Vector<S, T> a, Vector<S, T> b, T factor)
{
    static_assert(std::is_floating_point_v<T>);

    return a * (T(1.0) - factor) + b * factor;
}

//============================================================================//

/// Compute the component-wise sine of a vector.
template <int S, class T> inline
Vector<S, T> sin(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i)
        vec[i] = std::sin(vec[i]);

    return vec;
}

/// Compute the component-wise cosine of a vector.
template <int S, class T> inline
Vector<S, T> cos(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i)
        vec[i] = std::cos(vec[i]);

    return vec;
}

/// Compute the component-wise tangent of a vector.
template <int S, class T> inline
Vector<S, T> tan(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i)
        vec[i] = std::tan(vec[i]);

    return vec;
}

//============================================================================//

/// Compute the component-wise absolute of a vector.
template <int S, class T> constexpr
Vector<S, T> abs(Vector<S, T> vec)
{
    for (int i = 0; i < S; ++i)
        vec[i] = std::abs(vec[i]);

    return vec;
}

//============================================================================//

/// Convert a vector from cycles to radians.
template <int S, class T> constexpr
Vector<S, T> radians(Vector<S, T> cycles)
{
    static_assert(std::is_floating_point_v<T>);

    return cycles * T(2.0 * 3.14159265358979323846);
}

/// Convert a vector from radians to cycles.
template <int S, class T> constexpr
Vector<S, T> cycles(Vector<S, T> radians)
{
    static_assert(std::is_floating_point_v<T>);

    return radians * T(0.5 / 3.14159265358979323846);
}

//============================================================================//

/// Compute the dot product of two vectors.
template <int S, class T> constexpr
T dot(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    T result = T(0.0);

    for (int i = 0; i < S; ++i)
        result += a[i] * b[i];

    return result;
}

/// Compute the dot product of a vector and a scalar.
template <int S, class T> constexpr
T dot(Vector<S, T> v, T s)
{
    static_assert(std::is_floating_point_v<T>);

    T result = T(0.0);

    for (int i = 0; i < S; ++i)
        result += v[i] * s;

    return result;
}

//============================================================================//

/// Compute the squared length of a vector.
template <int S, class T> constexpr
T length_squared(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::dot(vec, vec);
}

/// Compute the length of a vector.
template <int S, class T> inline
T length(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return std::sqrt(maths::dot(vec, vec));
}

//============================================================================//

/// Compute the squared distance between two vectors.
template <int S, class T> constexpr
T distance_squared(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::length_squared(a - b);
}

/// Compute the distance between two vectors.
template <int S, class T> inline
T distance(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::length(a - b);
}

//============================================================================//

/// Compute the unit length vector.
template <int S, class T> inline
Vector<S, T> normalize(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return vec * (T(1.0) / maths::length(vec));
}

//============================================================================//

/// Compute the reflection of incident by normal.
template <int S, class T> constexpr
Vector<S, T> reflect(Vector<S, T> incident, Vector<S, T> normal)
{
    static_assert(std::is_floating_point_v<T>);

    return incident - normal * maths::dot(normal, incident) * T(2.0);
}

//============================================================================//

/// Compute the cross product of two vectors.
template <class T> constexpr
Vector3<T> cross(Vector3<T> a, Vector3<T> b)
{
    static_assert(std::is_floating_point_v<T>);

    T x = (a.y * b.z) - (b.y * a.z);
    T y = (a.z * b.x) - (b.z * a.x);
    T z = (a.x * b.y) - (b.x * a.y);
    return Vector3<T>(x, y, z);
}

//============================================================================//

/// Rotate a vector along the X axis.
template <class T> inline
Vector3<T> rotate_x(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T y = (+vec.y * c) + (-vec.z * s);
    T z = (+vec.y * s) + (+vec.z * c);
    return Vector3<T>(vec.x, y, z);
}

/// Rotate a vector along the Y axis.
template <class T> inline
Vector3<T> rotate_y(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (+vec.z * s);
    T z = (-vec.x * s) + (+vec.z * c);
    return Vector3<T>(x, vec.y, z);
}

/// Rotate a vector along the Z axis.
template <class T> inline
Vector3<T> rotate_z(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (-vec.y * s);
    T y = (+vec.x * s) + (+vec.y * c);
    return Vector3<T>(x, y, vec.z);
}

//============================================================================//

///// Rotate a vector along an arbitary axis.
//template <class T> constexpr
//Matrix33<T> rotation(Vector3<T> axis, T angle)
//{
//    const T c = std::cos(maths::radians(angle)), s = std::sin(maths::radians(angle));
//    const Vector3<T> x = axis * axis.x, y = axis * axis.y, z = axis * axis.z;

//    Vector3<T> colA { x.x + (T(1)-x.x) * c, x.y * (T(1)-c) - axis.z * s, x.z * (T(1)-c) + axis.y * s };
//    Vector3<T> colB { y.x * (T(1)-c) + axis.z * s, y.y + (T(1)-y.y) * c, y.z * (T(1)-c) - axis.x * s };
//    Vector3<T> colC { z.x * (T(1)-c) - axis.y * s, z.y * (T(1)-c) + axis.x * s, z.z + (T(1)-z.z) * c };

//    return Matrix33<T> { colA, colB, colC };
//}

//============================================================================//

}} // namespace sq::maths

using Vec2I = sq::Vector<2, int>;
using Vec3I = sq::Vector<3, int>;
using Vec4I = sq::Vector<4, int>;
using Vec2U = sq::Vector<2, uint>;
using Vec3U = sq::Vector<3, uint>;
using Vec4U = sq::Vector<4, uint>;
using Vec2F = sq::Vector<2, float>;
using Vec3F = sq::Vector<3, float>;
using Vec4F = sq::Vector<4, float>;
