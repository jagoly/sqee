#pragma once

#include <sqee/maths/Scalar.hpp>

namespace sq {

template<int S, class T, is_numeric<T>...> struct Vector {};

template<class T> using Vector2 = Vector<2, T>;
template<class T> using Vector3 = Vector<3, T>;
template<class T> using Vector4 = Vector<4, T>;


template<class T> struct Vector<2, T> {
    Vector() : x(T(0)), y(T(0)) {}
    Vector(T _x, T _y) : x(_x), y(_y) {}
    Vector(const Vector2<T>& _v) : x(_v.x), y(_v.y) {}

    template<class U> explicit Vector(Vector2<U> _v) : x(_v.x), y(_v.y) {}

    explicit Vector(T _s) : x(_s), y(_s) {}
    explicit Vector(Vector3<T> _v) : x(_v.x), y(_v.y) {}
    explicit Vector(Vector4<T> _v) : x(_v.x), y(_v.y) {}

    inline T& operator[](int _index) { return (&x)[_index]; }
    inline const T& operator[](int _index) const { return (&x)[_index]; }
    inline const T* ptr() const { return &x; }

    union {T x, r;}; union {T y, g;};
};


template<class T> struct Vector<3, T> {
    Vector() : x(T(0)), y(T(0)), z(T(0)) {}
    Vector(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vector(const Vector3<T>& _v) : x(_v.x), y(_v.y), z(_v.z) {}

    template<class U> explicit Vector(Vector3<U> _v) : x(_v.x), y(_v.y), z(_v.z) {}

    explicit Vector(T _s) : x(_s), y(_s), z(_s) {}
    explicit Vector(Vector4<T> _v) : x(_v.x), y(_v.y), z(_v.z) {}

    explicit Vector(Vector2<T> ab, T c) : x(ab.x), y(ab.y), z(c) {}
    explicit Vector(T a, Vector2<T> bc) : x(a), y(bc.x), z(bc.y) {}

    inline T& operator[](int _index) { return (&x)[_index]; }
    inline const T& operator[](int _index) const { return (&x)[_index]; }
    inline const T* ptr() const { return &x; }

    union {T x, r;}; union {T y, g;}; union {T z, b;};
};


template<class T> struct Vector<4, T> {
    Vector() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}
    Vector(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector(const Vector4<T>& _v) : x(_v.x), y(_v.y), z(_v.z), w(_v.w) {}

    template<class U> explicit Vector(Vector4<U> _v) : x(_v.x), y(_v.y), z(_v.z), w(_v.w) {}

    explicit Vector(T _s) : x(_s), y(_s), z(_s), w(_s) {}

    explicit Vector(Vector2<T> ab, T c, T d) : x(ab.x), y(ab.y), z(c), w(d) {}
    explicit Vector(T a, Vector2<T> bc, T d) : x(a), y(bc.x), z(bc.y), w(d) {}
    explicit Vector(T a, T b, Vector2<T> cd) : x(a), y(b), z(cd.x), w(cd.y) {}
    explicit Vector(Vector3<T> abc, T d) : x(abc.x), y(abc.y), z(abc.z), w(d) {}
    explicit Vector(T a, Vector3<T> bcd) : x(a), y(bcd.x), z(bcd.y), w(bcd.z) {}
    explicit Vector(Vector2<T> ab, Vector2<T> cd) : x(ab.x), y(ab.y), z(cd.x), w(cd.w)  {}

    inline T& operator[](int _index) { return (&x)[_index]; }
    inline const T& operator[](int _index) const { return (&x)[_index]; }
    inline const T* ptr() const { return &x; }

    union {T x, r;}; union {T y, g;}; union {T z, b;}; union {T w, a;};
};


// Vector Operators +, += /////
template<int S, class T> inline Vector<S, T> operator+(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _a[i]+_b[i]; return ret; }
template<int S, class T> inline Vector<S, T>& operator+=(Vector<S, T>& _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) _a[i] = _a[i]+_b[i]; return _a; }

// Vector Operators -, -= /////
template<int S, class T> inline Vector<S, T> operator-(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _a[i]-_b[i]; return ret; }
template<int S, class T> inline Vector<S, T>& operator-=(Vector<S, T>& _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) _a[i] = _a[i]-_b[i]; return _a; }

// Vector Operators *, *= /////
template<int S, class T> inline Vector<S, T> operator*(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _a[i]*_b[i]; return ret; }
template<int S, class T> inline Vector<S, T>& operator*=(Vector<S, T>& _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) _a[i] = _a[i]*_b[i]; return _a; }

// Vector Operators /, /= /////
template<int S, class T> inline Vector<S, T> operator/(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _a[i]/_b[i]; return ret; }
template<int S, class T> inline Vector<S, T>& operator/=(Vector<S, T>& _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) _a[i] = _a[i]/_b[i]; return _a; }

// Scalar Operators +, += /////
template<int S, class T> inline Vector<S, T> operator+(Vector<S, T> _v, T _s) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _v[i]+_s; return ret; }
template<int S, class T> inline Vector<S, T>& operator+=(Vector<S, T>& _v, T _s) {
    for (int i=0; i<S; ++i) _v[i] = _v[i]+_s; return _v; }

// Scalar Operators -, -= /////
template<int S, class T> inline Vector<S, T> operator-(Vector<S, T> _v, T _s) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _v[i]-_s; return ret; }
template<int S, class T> inline Vector<S, T>& operator-=(Vector<S, T>& _v, T _s) {
    for (int i=0; i<S; ++i) _v[i] = _v[i]-_s; return _v; }

// Scalar Operators *, *= /////
template<int S, class T> inline Vector<S, T> operator*(Vector<S, T> _v, T _s) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _v[i]*_s; return ret; }
template<int S, class T> inline Vector<S, T>& operator*=(Vector<S, T>& _v, T _s) {
    for (int i=0; i<S; ++i) _v[i] = _v[i]*_s; return _v; }

// Scalar Operators /, /= /////
template<int S, class T> inline Vector<S, T> operator/(Vector<S, T> _v, T _s) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = _v[i]/_s; return ret; }
template<int S, class T> inline Vector<S, T>& operator/=(Vector<S, T>& _v, T _s) {
    for (int i=0; i<S; ++i) _v[i] = _v[i]/_s; return _v; }

// Negative Operator /////
template<int S, class T> inline Vector<S, T> operator-(Vector<S, T> _v) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = -_v[i]; return ret;
}

// Equality Operator /////
template<int S, class T, is_integer<T>...>
inline bool operator==(Vector<S, T> _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) if (_a[i] != _b[i]) return false; return true;
}

// Comparison Operator /////
template<int S, class T, is_integer<T>...>
inline bool operator<(Vector<S, T> _a, Vector<S, T> _b) {
    for (int i=0; i<S; ++i) if (_a[i] != _b[i]) return _a[i] < _b[i]; return false;
}


namespace maths {

// Minimum /////
template<int S, class T> inline Vector<S, T> min(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = min(_a[i], _b[i]); return ret;
}

// Maximum /////
template<int S, class T> inline Vector<S, T> max(Vector<S, T> _a, Vector<S, T> _b) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = max(_a[i], _b[i]); return ret;
}

// Sine /////
template<int S, class T, is_float<T>...> inline Vector<S, T> sin(Vector<S, T> _v) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = std::sin(_v[i]); return ret;
}

// Cosine /////
template<int S, class T, is_float<T>...> inline Vector<S, T> cos(Vector<S, T> _v) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = std::cos(_v[i]); return ret;
}

// Tangent /////
template<int S, class T, is_float<T>...> inline Vector<S, T> tan(Vector<S, T> _v) {
    Vector<S, T> ret; for (int i=0; i<S; ++i) ret[i] = std::tan(_v[i]); return ret;
}

// To Radians /////
template<int S, class T, is_float<T>...>
inline Vector<S, T> radians(Vector<S, T> _cycles) {
    return _cycles * T(2.0 * M_PI);
}

// To Cycles /////
template<int S, class T, is_float<T>...>
inline Vector<S, T> cycles(Vector<S, T> _radians) {
    return _radians * T(0.5 / M_PI);
}

// Dot Product /////
template<int S, class T, is_float<T>...>
inline T dot(Vector<S, T> _a, Vector<S, T> _b) {
    T ret(0); for (int i=0; i<S; ++i) ret += _a[i]*_b[i]; return ret;
}

// Linear Mix /////
template<int S, class T, is_float<T>...>
inline Vector<S, T> mix(Vector<S, T> _a, Vector<S, T> _b, T _factor) {
    return _a * (T(1.0) - _factor) + _b * _factor;
}

// Normalize /////
template<int S, class T, is_float<T>...>
inline Vector<S, T> normalize(Vector<S, T> _v) {
    return _v * (T(1.0) / std::sqrt(dot(_v, _v)));
}

// Length /////
template<int S, class T, is_float<T>...>
inline T length(Vector<S, T> _v) {
    return std::sqrt(dot(_v, _v));
}

// Distance /////
template<int S, class T, is_float<T>...>
inline T distance(Vector<S, T> _a, Vector<S, T> _b) {
    return length(_a - _b);
}

// Reflect /////
template<int S, class T, is_float<T>...>
inline Vector<S, T> reflect(Vector<S, T> _incident, Vector<S, T> _normal) {
    return _incident - _normal * dot(_normal, _incident) * T(2.0);
}

// Cross Product /////
template<class T, is_float<T>...>
inline Vector3<T> cross(Vector3<T> _a, Vector3<T> _b) {
    return Vector3<T>(_a.y*_b.z - _b.y*_a.z, _a.z*_b.x - _b.z*_a.x, _a.x*_b.y - _b.x*_a.y);
}

// Rotate X Axis /////
template<class T, is_float<T>...>
inline Vector3<T> rotate_x(Vector3<T> _v, T _angle) {
    T a = radians(_angle), s = std::sin(a), c = std::cos(a);
    return Vector3<T>(_v.x, +_v.y*c -_v.z*s, +_v.y*s +_v.z*c);
}

// Rotate Y Axis /////
template<class T, is_float<T>...>
inline Vector3<T> rotate_y(Vector3<T> _v, T _angle) {
    T a = radians(_angle), s = std::sin(a), c = std::cos(a);
    return Vector3<T>(+_v.x*c +_v.z*s, _v.y, -_v.x*s +_v.z*c);
}

// Rotate Z Axis /////
template<class T, is_float<T>...>
inline Vector3<T> rotate_z(Vector3<T> _v, T _angle) {
    T a = radians(_angle), s = std::sin(a), c = std::cos(a);
    return Vector3<T>(+_v.x*c -_v.y*s, +_v.x*s +_v.y*c, _v.z);
}

}}

using Vec2I = sq::Vector<2, int>;
using Vec3I = sq::Vector<3, int>;
using Vec4I = sq::Vector<4, int>;
using Vec2U = sq::Vector<2, uint>;
using Vec3U = sq::Vector<3, uint>;
using Vec4U = sq::Vector<4, uint>;
using Vec2F = sq::Vector<2, float>;
using Vec3F = sq::Vector<3, float>;
using Vec4F = sq::Vector<4, float>;
