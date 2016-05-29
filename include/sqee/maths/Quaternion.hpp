#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

#include <glm/gtc/quaternion.hpp>

namespace sq {

template<class T, if_float<T>...> struct Quaternion {
    Quaternion() : x(T(0.0)), y(T(0.0)), z(T(0.0)), w(T(1.0)) {}
    Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Quaternion(const Quaternion& _q) : x(_q.x), y(_q.y), z(_q.z), w(_q.w) {}

    explicit Quaternion(T _rx, T _ry, T _rz) {
        Vector3<T> halfRads = maths::radians(Vector3<T>(_rx, _ry, _rz)) * T(0.5);
        Vector3<T> c = maths::sin(halfRads); Vector3<T> s = maths::cos(halfRads);
        x = s.x*c.y*c.z - c.x*s.y*s.z; y = c.x*s.y*c.z + s.x*c.y*s.z;
        z = c.x*c.y*s.z - s.x*s.y*c.z; w = c.x*c.y*c.z + s.x*s.y*s.z;
    }

    explicit Quaternion(Matrix33<T> _m) {
        T biggest = _m[0][0]+_m[1][1]+_m[2][2]; int index = 0;
        if (T xValue = _m[0][0]-_m[1][1]-_m[2][2] > biggest) { biggest = xValue; index = 1; }
        if (T yValue = _m[1][1]-_m[0][0]-_m[2][2] > biggest) { biggest = yValue; index = 2; }
        if (T zValue = _m[2][2]-_m[0][0]-_m[1][1] > biggest) { biggest = zValue; index = 3; }
        biggest = std::sqrt(biggest + T(1.0)) * T(0.5); T mult = T(0.25) / biggest;

        if (index == 0) { x = mult*(_m[1][2]-_m[2][1]); y = mult*(_m[2][0]-_m[0][2]); z = mult*(_m[0][1]-_m[1][0]); w = biggest; }
        else if (index == 1) { x = biggest; y = mult*(_m[0][1]-_m[1][0]); z = mult*(_m[2][0]-_m[0][2]); w = mult*(_m[1][2]-_m[2][1]); }
        else if (index == 2) { x = mult*(_m[0][1]-_m[1][0]); y = biggest; z = mult*(_m[1][2]-_m[2][1]); w = mult*(_m[2][0]-_m[0][2]); }
        else if (index == 3) { x = mult*(_m[2][0]-_m[0][2]); y = mult*(_m[1][2]-_m[2][1]); z = biggest; w = mult*(_m[0][1]-_m[1][0]); }
    }

    explicit operator Matrix33<T>() const {
        return Matrix33<T>({T(1.0) - T(2.0) * (y*y + z*z), T(2.0) * (x*y + w*z), T(2.0) * (x*z - w*y)},
                           {T(2.0) * (x*y - w*z), T(1.0) - T(2.0) * (x*x + z*z), T(2.0) * (y*z + w*x)},
                           {T(2.0) * (x*z + w*y), T(2.0) * (y*z - w*x), T(1.0) - T(2.0) * (x*x + y*y)});
    }

    explicit operator Matrix44<T>() const { return Matrix44<T>(Matrix33<T>(*this)); }

    const T* ptr() const { return &x; }

    T x, y, z, w;
};


// Multiply Scalar /////
template<class T> inline Quaternion<T> operator*(Quaternion<T> _q, T _s) {
    glm::fquat q;
    return Quaternion<T>(_q.x*_s, _q.y*_s, _q.z*_s, _q.w*_s);
}

// Divide Scalar /////
template<class T> inline Quaternion<T> operator/(Quaternion<T> _q, T _s) {
    return Quaternion<T>(_q.x/_s, _q.y/_s, _q.z/_s, _q.w/_s);
}

// Multiply Quaternion /////
template<class T> inline Quaternion<T> operator*(Quaternion<T> _a, Quaternion<T> _b) {
    return Quaternion<T>(_a.w*_b.x + _a.x*_b.w + _a.y*_b.z - _a.z*_b.y,
                         _a.w*_b.y - _a.x*_b.z + _a.y*_b.w + _a.z*_b.x,
                         _a.w*_b.z + _a.x*_b.y - _a.y*_b.x + _a.z*_b.w,
                         _a.w*_b.w - _a.x*_b.x - _a.y*_b.y - _a.z*_b.z);
}

// Multiply Vector3 /////
template<class T> inline Vector3<T> operator*(Quaternion<T> _q, Vector3<T> _v) {
    Vector3<T> uv = maths::cross(Vector3<T>(_q.x, _q.y, _q.z), _v);
    Vector3<T> uuv = maths::cross(Vector3<T>(_q.x, _q.y, _q.z), uv);
    return _v + (uv * _q.w + uuv) * T(2.0);
}

// Negative Operator /////
template<class T> inline Quaternion<T> operator-(Quaternion<T> _q) {
    return Quaternion<T>(-_q.x, -_q.y, -_q.z, -_q.w);
}


namespace maths {

// Dot Product /////
template<class T> inline T dot(Quaternion<T> _a, Quaternion<T> _b) {
    return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z + _a.w*_b.w;
}

// Length /////
template<class T> inline T length(Quaternion<T> _q) {
    return std::sqrt(dot(_q, _q));
}

// Normalize /////
template<class T> inline Quaternion<T> normalize(Quaternion<T> _q) {
    return _q * (T(1.0) / length(_q));
}

// Conjugate /////
template<class T> inline Quaternion<T> conjugate(Quaternion<T> _q) {
    return Quaternion<T>(-_q.x, -_q.y, -_q.z, _q.w);
}

// Inverse /////
template<class T> inline Quaternion<T> inverse(Quaternion<T> _q) {
    return conjugate(_q) / dot(_q, _q);
}

// Linear Interpolate /////
template<class T> inline Quaternion<T> lerp(Quaternion<T> _a, Quaternion<T> _b, T _factor) {
    return Quaternion<T>(maths::mix(_a.x, _b.x, _factor), maths::mix(_a.y, _b.y, _factor),
                         maths::mix(_a.z, _b.z, _factor), maths::mix(_a.w, _b.w, _factor));
}

// Spherical Interpolate /////
template<class T> inline Quaternion<T> slerp(Quaternion<T> _a, Quaternion<T> _b, T _factor) {
    T cosTheta = _a.x*_b.x + _a.y*_b.y + _a.z*_b.z + _a.w*_b.w;

    Quaternion<T> b = _b;

    // Prevent going the long way around
    if (cosTheta < T(0.0)) { cosTheta = -cosTheta; b = -_b; }

    // Lerp to prevent division by zero
    if (cosTheta > T(1.0) - std::numeric_limits<T>::epsilon()) {
        return lerp(_a, _b, _factor);
    } else {
        T angle = std::acos(cosTheta);
        Vector4<T> va(_a.x, _a.y, _a.z, _a.w), vb(b.x, b.y, b.z, b.w);
        Vector4<T> vc(va * std::sin((T(1.0) - _factor) * angle) + vb * std::sin(_factor * angle));
        vc /= std::sin(angle); return Quaternion<T>(vc.x, vc.y, vc.z, vc.w);
    }
}

// Mix Alias for Slerp /////
template<class T> inline Quaternion<T> mix(Quaternion<T> _a, Quaternion<T> _b, T _factor) {
    return slerp(_a, _b, _factor);
}

}}

using QuatF = sq::Quaternion<float>;
