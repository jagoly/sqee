#pragma once

#include <sqee/maths/Scalar.hpp>

namespace sq {

//============================================================================//

template <int S, class T, if_numeric<T>...> struct Vector {};

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
    constexpr Vector(const Vector2<T>& v) : data { v[0], v[1] } {}
    template<class U> explicit Vector(Vector2<U> v) : Vector(v.x, v.y) {}

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
    constexpr Vector(const Vector3<T>& v) : data { v[0], v[1], v[2] } {}
    template<class U> explicit Vector(Vector3<U> v) : Vector(v.x, v.y, v.z) {}

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
    constexpr Vector(const Vector4<T>& v) : data { v[0], v[1], v[2], v[3] } {}
    template<class U> explicit Vector(Vector4<U> v) : Vector(v.x, v.y, v.z, v.w) {}

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
    for (int i=0; i<S; ++i) if (a[i] != b[i]) return false; return true;
}

template <int S, class T> constexpr
bool operator!=(Vector<S, T> a, Vector<S, T> b)
{
    for (int i=0; i<S; ++i) if (a[i] != b[i]) return true; return false;
}

//============================================================================//

// integral comparison (Vector, Vector) /////

template <int S, class T> constexpr
bool operator<(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_integral<T>::value, "should not compare floats");
    for (int i=0; i<S; ++i) if (a[i] != b[i]) return a[i] < b[i]; return false;
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

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] -= b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator-=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] -= b[i]; return a; }

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator*(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] *= b[i]; return a; }

template <int S, class T> constexpr
Vector<S, T>& operator*=(Vector<S, T>& a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] *= b[i]; return a; }

//========================================================//

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

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] -= s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator-=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] -= s; return v; }

//========================================================//

template <int S, class T> constexpr
Vector<S, T> operator*(Vector<S, T> v, T s)
{ for (int i=0; i<S; ++i) v[i] *= s; return v; }

template <int S, class T> constexpr
Vector<S, T>& operator*=(Vector<S, T>& v, T s)
{ for (int i=0; i<S; ++i) v[i] *= s; return v; }

//========================================================//

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
    static_assert(std::is_signed<T>::value, "");
    return Vector<S, T>() + vec;
}

template <int S, class T> constexpr
Vector<S, T> operator-(Vector<S, T> vec)
{
    static_assert(std::is_signed<T>::value, "");
    return Vector<S, T>() - vec;
}

//============================================================================//

namespace maths {

//============================================================================//

// minimum, maximum (Vectors) /////

template <int S, class T> constexpr
Vector<S, T> min(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] = min(a[i], b[i]); return a; }

template <int S, class T> constexpr
Vector<S, T> max(Vector<S, T> a, Vector<S, T> b)
{ for (int i=0; i<S; ++i) a[i] = max(a[i], b[i]); return a; }

//============================================================================//

// clamp to range (Vector) /////

template <int S, class T> constexpr
Vector<S, T> clamp(Vector<S, T> vec, Vector<S, T> min, Vector<S, T> max)
{
    return maths::min(maths::max(vec, min), max);
}

//============================================================================//

// linear interpolation (Vectors, Scalar) /////

template <int S, class T, if_float<T>...> constexpr
Vector<S, T> mix(Vector<S, T> a, Vector<S, T> b, T factor)
{
    return a * (T(1.0) - factor) + b * factor;
}

//============================================================================//

// sine, cosine, tangent (Vector) /////

template <int S, class T, if_float<T>...> inline
Vector<S, T> sin(Vector<S, T> vec)
{ for (int i=0; i<S; ++i) vec[i] = std::sin(vec[i]); return vec; }

template <int S, class T, if_float<T>...> inline
Vector<S, T> cos(Vector<S, T> vec)
{ for (int i=0; i<S; ++i) vec[i] = std::cos(vec[i]); return vec; }

template <int S, class T, if_float<T>...> inline
Vector<S, T> tan(Vector<S, T> vec)
{ for (int i=0; i<S; ++i) vec[i] = std::tan(vec[i]); return vec; }

//============================================================================//

// absolute (Vector) /////

template <int S, class T, if_numeric<T>...> constexpr
Vector<S, T> abs(Vector<S, T> vec)
{ for (int i=0; i<S; ++i) vec[i] = std::abs(vec[i]); return vec; }

//============================================================================//

// convert between radians and cycles (Vector) /////

template <int S, class T, if_float<T>...> constexpr
Vector<S, T> radians(Vector<S, T> cycles)
{ return cycles * T(2.0 * 3.14159265358979323846); }

template <int S, class T, if_float<T>...> constexpr
Vector<S, T> cycles(Vector<S, T> radians)
{ return radians * T(0.5 / 3.14159265358979323846); }

//============================================================================//

// dot product (Vector, Vector) /////

template <int S, class T, if_float<T>...> constexpr
T dot(Vector<S, T> a, Vector<S, T> b)
{
    T result = T(0.0);

    for (int i = 0; i < S; ++i)
        result += a[i] * b[i];

    return result;
}

//============================================================================//

// dot product (Vector, Scalar) /////

template <int S, class T, if_float<T>...> constexpr
T dot(Vector<S, T> v, T s)
{
    T result = T(0.0);

    for (int i = 0; i < S; ++i)
        result += v[i] * s;

    return result;
}

//============================================================================//

// length (Vector) /////

template <int S, class T, if_float<T>...> inline
T length(Vector<S, T> vec)
{
    return std::sqrt(maths::dot(vec, vec));
}

//============================================================================//

// normalize (Vector) /////

template <int S, class T, if_float<T>...> inline
Vector<S, T> normalize(Vector<S, T> vec)
{
    return vec * (T(1.0) / maths::length(vec));
}

//============================================================================//

// distance (Vector, Vector) /////

template <int S, class T, if_float<T>...> inline
T distance(Vector<S, T> a, Vector<S, T> b)
{
    return maths::length(a - b);
}

//============================================================================//

// reflect incident ray (Vector, Vector) /////

template <int S, class T, if_float<T>...> constexpr
Vector<S, T> reflect(Vector<S, T> incident, Vector<S, T> normal)
{
    return incident - normal * maths::dot(normal, incident) * T(2.0);
}

//============================================================================//

// cross product (Vector3, Vector3) /////

template <class T, if_float<T>...> constexpr
Vector3<T> cross(Vector3<T> a, Vector3<T> b)
{
    T x = (a.y * b.z) - (b.y * a.z);
    T y = (a.z * b.x) - (b.z * a.x);
    T z = (a.x * b.y) - (b.x * a.y);
    return Vector3<T>(x, y, z);
}

//============================================================================//

// rotate along axis (Vector3, Scalar) /////

template <class T, if_float<T>...> inline
Vector3<T> rotate_x(Vector3<T> vec, T angle)
{
    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T y = (+vec.y * c) + (-vec.z * s);
    T z = (+vec.y * s) + (+vec.z * c);
    return Vector3<T>(vec.x, y, z);
}

template <class T, if_float<T>...> inline
Vector3<T> rotate_y(Vector3<T> vec, T angle)
{
    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (+vec.z * s);
    T z = (-vec.x * s) + (+vec.z * c);
    return Vector3<T>(x, vec.y, z);
}

template <class T, if_float<T>...> inline
Vector3<T> rotate_z(Vector3<T> vec, T angle)
{
    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (-vec.y * s);
    T y = (+vec.x * s) + (+vec.y * c);
    return Vector3<T>(x, y, vec.z);
}

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
