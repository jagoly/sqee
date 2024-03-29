#pragma once

#include <sqee/maths/Scalar.hpp>

namespace sq::maths { //########################################################

template <int S, class T> struct Vector;

template <class T> using Vector2 = Vector<2, T>;
template <class T> using Vector3 = Vector<3, T>;
template <class T> using Vector4 = Vector<4, T>;

/// Concept that matches any kind of vector.
template <class VectorST>
concept AnyVector = requires (VectorST vec)
{
    []<int S, class T>(Vector<S, T>&){}(vec);
};

//==============================================================================

template <class T> struct Vector<2, T>
{
    T x, y;

    constexpr Vector() : x(0), y(0) {}
    constexpr Vector(T x, T y) : x(x), y(y) {}

    constexpr explicit Vector(T s) : Vector(s, s) {}

    constexpr Vector(const Vector& v) = default;
    constexpr Vector& operator=(const Vector& v) = default;

    /// Construct without initialisation.
    Vector(std::nullptr_t) {}

    /// Construct from a vector of a different scalar type.
    template <class U>
    constexpr explicit Vector(Vector2<U> v) : Vector(T(v.x), T(v.y)) {}

    constexpr explicit Vector(Vector3<T> v) : Vector(v.x, v.y) {}
    constexpr explicit Vector(Vector4<T> v) : Vector(v.x, v.y) {}

    constexpr const T& operator[](int index) const
    {
        if (std::is_constant_evaluated())
        {
            if (index == 0) return x;
            if (index == 1) return y;
        }
        return reinterpret_cast<const T*>(this)[index];
    }

    constexpr T& operator[](int index)
    {
        return const_cast<T&>(static_cast<const Vector&>(*this)[index]);
    }

    const T* data() const { return reinterpret_cast<const T*>(this); }

    T* data() { return reinterpret_cast<T*>(this); }
};

template <class T> Vector(T, T) -> Vector<2, T>;

//==============================================================================

template <class T> struct Vector<3, T>
{
    T x, y, z;

    constexpr Vector() : x(0), y(0), z(0) {}
    constexpr Vector(T x, T y, T z) : x(x), y(y), z(z) {}

    constexpr explicit Vector(T s) : Vector(s, s, s) {}

    constexpr Vector(const Vector& v) = default;
    constexpr Vector& operator=(const Vector& v) = default;

    /// Construct without initialisation.
    Vector(std::nullptr_t) {}

    /// Construct from a vector of a different scalar type.
    template <class U>
    constexpr explicit Vector(Vector3<U> v) : Vector(T(v.x), T(v.y), T(v.z)) {}

    constexpr explicit Vector(Vector4<T> v) : Vector(v.x, v.y, v.z) {}
    constexpr explicit Vector(Vector2<T> xy, T z) : Vector(xy.x, xy.y, z) {}
    constexpr explicit Vector(T x, Vector2<T> yz) : Vector(x, yz.x, yz.y) {}

    constexpr const T& operator[](int index) const
    {
        if (std::is_constant_evaluated())
        {
            if (index == 0) return x;
            if (index == 1) return y;
            if (index == 2) return z;
        }
        return reinterpret_cast<const T*>(this)[index];
    }

    constexpr T& operator[](int index)
    {
        return const_cast<T&>(static_cast<const Vector&>(*this)[index]);
    }

    const T* data() const { return reinterpret_cast<const T*>(this); }

    T* data() { return reinterpret_cast<T*>(this); }
};

template <class T> Vector(T, T, T) -> Vector<3, T>;
template <class T> Vector(Vector2<T>, T) -> Vector<3, T>;
template <class T> Vector(T, Vector2<T>) -> Vector<3, T>;

//==============================================================================

template <class T> struct Vector<4, T>
{
    T x, y, z, w;

    constexpr Vector() : x(0), y(0), z(0), w(0) {}
    constexpr Vector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    constexpr explicit Vector(T s) : Vector(s, s, s, s) {}

    constexpr Vector(const Vector& v) = default;
    constexpr Vector& operator=(const Vector& v) = default;

    /// Construct without initialisation.
    Vector(std::nullptr_t) {}

    /// Construct from a vector of a different scalar type.
    template <class U>
    explicit Vector(Vector4<U> v) : Vector(T(v.x), T(v.y), T(v.z), T(v.w)) {}

    constexpr explicit Vector(Vector2<T> xy, T z, T w) : Vector(xy.x, xy.y, z, w) {}
    constexpr explicit Vector(T x, Vector2<T> yz, T w) : Vector(x, yz.x, yz.y, w) {}
    constexpr explicit Vector(T x, T y, Vector2<T> zw) : Vector(x, y, zw.x, zw.y) {}
    constexpr explicit Vector(Vector3<T> xyz, T w) : Vector(xyz.x, xyz.y, xyz.z, w) {}
    constexpr explicit Vector(T x, Vector3<T> yzw) : Vector(x, yzw.x, yzw.y, yzw.z) {}

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
        return const_cast<T&>(static_cast<const Vector&>(*this)[index]);
    }

    const T* data() const { return reinterpret_cast<const T*>(this); }

    T* data() { return reinterpret_cast<T*>(this); }
};

template <class T> Vector(T, T, T, T) -> Vector<4, T>;
template <class T> Vector(Vector2<T>, T, T) -> Vector<4, T>;
template <class T> Vector(T, Vector2<T>, T) -> Vector<4, T>;
template <class T> Vector(T, T, Vector2<T>) -> Vector<4, T>;
template <class T> Vector(Vector3<T>, T) -> Vector<4, T>;
template <class T> Vector(T, Vector3<T>) -> Vector<4, T>;

//==============================================================================

template <int S, class T>
constexpr auto operator<=>(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        if (auto cmp = a[i] <=> b[i]; cmp != 0) return cmp;
    return T() <=> T();
}

template <int S, class T>
constexpr bool operator==(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i)
        if (a[i] != b[i]) return false;
    return true;
}

//==============================================================================

template <int S, class T>
constexpr Vector<S, T> operator+(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] += b[i];
    return a;
}
template <int S, class T>
constexpr Vector<S, T>& operator+=(Vector<S, T>& a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] += b[i];
    return a;
}

template <int S, class T>
constexpr Vector<S, T> operator-(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] -= b[i];
    return a;
}
template <int S, class T>
constexpr Vector<S, T>& operator-=(Vector<S, T>& a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] -= b[i];
    return a;
}

template <int S, class T>
constexpr Vector<S, T> operator*(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] *= b[i];
    return a;
}
template <int S, class T>
constexpr Vector<S, T>& operator*=(Vector<S, T>& a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] *= b[i];
    return a;
}

template <int S, class T>
constexpr Vector<S, T> operator/(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] /= b[i];
    return a;
}
template <int S, class T>
constexpr Vector<S, T>& operator/=(Vector<S, T>& a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] /= b[i];
    return a;
}

//==============================================================================

template <int S, class T>
constexpr Vector<S, T> operator+(Vector<S, T> v, T s)
{
    for (int i = 0; i < S; ++i) v[i] += s;
    return v;
}
template <int S, class T>
constexpr Vector<S, T>& operator+=(Vector<S, T>& v, T s)
{
    for (int i = 0; i < S; ++i) v[i] += s;
    return v;
}

template <int S, class T>
constexpr Vector<S, T> operator-(Vector<S, T> v, T s)
{
    for (int i = 0; i < S; ++i) v[i] -= s;
    return v;
}
template <int S, class T>
constexpr Vector<S, T>& operator-=(Vector<S, T>& v, T s)
{
    for (int i = 0; i < S; ++i) v[i] -= s;
    return v;
}

template <int S, class T>
constexpr Vector<S, T> operator*(Vector<S, T> v, T s)
{
    for (int i = 0; i < S; ++i) v[i] *= s;
    return v;
}
template <int S, class T>
constexpr Vector<S, T>& operator*=(Vector<S, T>& v, T s)
{
    for (int i = 0; i < S; ++i) v[i] *= s;
    return v;
}

template <int S, class T>
constexpr Vector<S, T> operator/(Vector<S, T> v, T s)
{
    for (int i = 0; i < S; ++i) v[i] /= s;
    return v;
}
template <int S, class T>
constexpr Vector<S, T>& operator/=(Vector<S, T>& v, T s)
{
    for (int i = 0; i < S; ++i) v[i] /= s;
    return v;
}

//==============================================================================

template <int S, class T>
constexpr Vector<S, T> operator+(Vector<S, T> vec)
{
    static_assert(std::is_signed_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = +vec[i];
    return vec;
}

template <int S, class T>
constexpr Vector<S, T> operator-(Vector<S, T> vec)
{
    static_assert(std::is_signed_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = -vec[i];
    return vec;
}

//==============================================================================

/// Compute the component-wise absolute of a vector.
template <int S, class T>
constexpr Vector<S, T> abs(Vector<S, T> vec)
{
    static_assert(std::is_signed_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = maths::abs(vec[i]);
    return vec;
}

/// Compute the component-wise minimum of two vectors.
template <int S, class T>
constexpr Vector<S, T> min(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] = maths::min(a[i], b[i]);
    return a;
}

/// Compute the component-wise maximum of two vectors.
template <int S, class T>
constexpr Vector<S, T> max(Vector<S, T> a, Vector<S, T> b)
{
    for (int i = 0; i < S; ++i) a[i] = maths::max(a[i], b[i]);
    return a;
}

/// Clamp a vector to within the specified range.
template <int S, class T>
constexpr Vector<S, T> clamp(Vector<S, T> vec, Vector<S, T> min, Vector<S, T> max)
{
    return maths::min(maths::max(vec, min), max);
}

//==============================================================================

/// Linearly interpolate between two vectors.
template <int S, class T>
constexpr Vector<S, T> mix(Vector<S, T> a, Vector<S, T> b, T factor)
{
    static_assert(std::is_floating_point_v<T>);

    return a * (T(1.0) - factor) + b * factor;
}

/// Interpolate between euler rotations component-wise.
template <class T>
constexpr Vector3<T> mix_radians(Vector3<T> a, Vector3<T> b, T factor)
{
    static_assert(std::is_floating_point_v<T>);

    T x = maths::mix_radians(a.x, b.x, factor);
    T y = maths::mix_radians(a.y, b.y, factor);
    T z = maths::mix_radians(a.z, b.z, factor);
    return { x, y, z };
}

//==============================================================================

/// Compute the component-wise sine of a vector.
template <int S, class T>
inline Vector<S, T> sin(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = std::sin(vec[i]);
    return vec;
}

/// Compute the component-wise cosine of a vector.
template <int S, class T>
inline Vector<S, T> cos(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = std::cos(vec[i]);
    return vec;
}

/// Compute the component-wise tangent of a vector.
template <int S, class T>
inline Vector<S, T> tan(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    for (int i = 0; i < S; ++i) vec[i] = std::tan(vec[i]);
    return vec;
}

//==============================================================================

/// Convert a vector from cycles to radians.
template <int S, class T>
constexpr Vector<S, T> radians(Vector<S, T> cycles)
{
    static_assert(std::is_floating_point_v<T>);

    return cycles * T(2.0 * 3.14159265358979323846);
}

/// Convert a vector from radians to cycles.
template <int S, class T>
constexpr Vector<S, T> cycles(Vector<S, T> radians)
{
    static_assert(std::is_floating_point_v<T>);

    return radians * T(0.5 / 3.14159265358979323846);
}

//==============================================================================

/// Compute the dot product of two vectors.
template <int S, class T>
constexpr T dot(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    T result = T(0.0);
    for (int i = 0; i < S; ++i) result += a[i] * b[i];
    return result;
}

/// Compute the dot product of a vector and a scalar.
template <int S, class T>
constexpr T dot(Vector<S, T> v, T s)
{
    static_assert(std::is_floating_point_v<T>);

    T result = T(0.0);
    for (int i = 0; i < S; ++i) result += v[i] * s;
    return result;
}

//==============================================================================

/// Compute the squared length of a vector.
template <int S, class T>
constexpr T length_squared(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::dot(vec, vec);
}

/// Compute the length of a vector.
template <int S, class T>
inline T length(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return std::sqrt(maths::dot(vec, vec));
}

/// Scale a vector to a length of one.
template <int S, class T>
inline Vector<S, T> normalize(Vector<S, T> vec)
{
    static_assert(std::is_floating_point_v<T>);

    return vec * (T(1.0) / maths::length(vec));
}

//==============================================================================

/// Compute the squared distance between two vectors.
template <int S, class T>
constexpr T distance_squared(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::length_squared(a - b);
}

/// Compute the distance between two vectors.
template <int S, class T>
inline T distance(Vector<S, T> a, Vector<S, T> b)
{
    static_assert(std::is_floating_point_v<T>);

    return maths::length(a - b);
}

//==============================================================================

/// Compute the reflection of incident by normal.
template <class T>
constexpr Vector3<T> reflect(Vector3<T> incident, Vector3<T> normal)
{
    static_assert(std::is_floating_point_v<T>);

    return incident - normal * maths::dot(normal, incident) * T(2.0);
}

/// Compute the cross product of two vectors.
template <class T>
constexpr Vector3<T> cross(Vector3<T> a, Vector3<T> b)
{
    static_assert(std::is_floating_point_v<T>);

    T x = (a.y * b.z) - (b.y * a.z);
    T y = (a.z * b.x) - (b.z * a.x);
    T z = (a.x * b.y) - (b.x * a.y);
    return { x, y, z };
}

//==============================================================================

/// Rotate a vector along the X axis.
template <class T>
inline Vector3<T> rotate_x(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T y = (+vec.y * c) + (-vec.z * s);
    T z = (+vec.y * s) + (+vec.z * c);
    return { vec.x, y, z };
}

/// Rotate a vector along the Y axis.
template <class T>
inline Vector3<T> rotate_y(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (+vec.z * s);
    T z = (-vec.x * s) + (+vec.z * c);
    return { x, vec.y, z };
}

/// Rotate a vector along the Z axis.
template <class T>
inline Vector3<T> rotate_z(Vector3<T> vec, T angle)
{
    static_assert(std::is_floating_point_v<T>);

    T s = std::sin(radians(angle));
    T c = std::cos(radians(angle));
    T x = (+vec.x * c) + (-vec.y * s);
    T y = (+vec.x * s) + (+vec.y * c);
    return { x, y, vec.z };
}

} // namespace sq::maths #######################################################

#include <sqee/core/TypeNames.hpp>

#include <fmt/format.h>

template <int S, class T>
struct fmt::formatter<sq::maths::Vector<S, T>>
{
    fmt::formatter<T> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    FormatContext::iterator format(const sq::maths::Vector<S, T>& vec, FormatContext& ctx) const
    {
        ctx.advance_to(fmt::detail::write(ctx.out(), sq::type_name_v<sq::maths::Vector<S, T>>.c_str()));
        ctx.advance_to(fmt::detail::write(ctx.out(), '('));
        ctx.advance_to(base.format(vec.x, ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(vec.y, ctx));
        if constexpr (S >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S >= 3) ctx.advance_to(base.format(vec.z, ctx));
        if constexpr (S == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S == 4) ctx.advance_to(base.format(vec.w, ctx));
        return fmt::detail::write(ctx.out(), ')');
    }
};
