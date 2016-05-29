#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

template<int W, int H, class T, if_float<T>...> struct Matrix {};

template<class T> using Matrix22 = Matrix<2, 2, T>;
template<class T> using Matrix23 = Matrix<2, 3, T>;
template<class T> using Matrix33 = Matrix<3, 3, T>;
template<class T> using Matrix34 = Matrix<3, 4, T>;
template<class T> using Matrix44 = Matrix<4, 4, T>;


template<class T> struct Matrix<2, 2, T> {
    Matrix() : cols{{1, 0}, {0, 1}} {}
    Matrix(const Matrix& _m) : cols{_m[0], _m[1]} {}
    using ColumnT = Vector<2, T>;

    explicit Matrix(T _s) : cols{{_s, 0}, {0, _s}} {}
    explicit Matrix(ColumnT _x, ColumnT _y) : cols{_x, _y} {}

    explicit Matrix(Matrix23<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}
    explicit Matrix(Matrix33<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}
    explicit Matrix(Matrix34<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}
    explicit Matrix(Matrix44<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}

    template<class U> explicit Matrix(Matrix22<U> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}

    ColumnT& operator[](int _index) { return cols[_index]; }
    const ColumnT& operator[](int _index) const { return cols[_index]; }
    Matrix& operator*=(Matrix _m) { return *this = (*this * _m); }
    const T* ptr() const { return &cols[0].x; }

    private: Vector<2, T> cols[2];
};


template<class T> struct Matrix<2, 3, T> {
    Matrix() : cols{{1, 0, 0}, {0, 1, 0}} {}
    Matrix(const Matrix& _m) : cols{_m[0], _m[1], _m[2]} {}
    using ColumnT = Vector<3, T>;

    explicit Matrix(T _s) : cols{{_s, 0, 0}, {0, _s, 0}} {}
    explicit Matrix(ColumnT _x, ColumnT _y) : cols{_x, _y} {}

    explicit Matrix(Matrix22<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0)} {}
    explicit Matrix(Matrix33<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}
    explicit Matrix(Matrix34<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}
    explicit Matrix(Matrix44<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}

    template<class U> explicit Matrix(Matrix23<U> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1])} {}

    ColumnT& operator[](int _index) { return cols[_index]; }
    const ColumnT& operator[](int _index) const { return cols[_index]; }
    //Matrix& operator*=(Matrix _m) { return *this = (*this * _m); }
    const T* ptr() const { return &cols[0].x; }

    private: Vector<3, T> cols[2];
};


template<class T> struct Matrix<3, 3, T> {
    Matrix() : cols{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} {}
    Matrix(const Matrix& _m) : cols{_m[0], _m[1], _m[2]} {}
    using ColumnT = Vector<3, T>;

    explicit Matrix(T _s) : cols{{_s, 0, 0}, {0, _s, 0}, {0, 0, _s}} {}
    explicit Matrix(ColumnT _x, ColumnT _y, ColumnT _z) : cols{_x, _y, _z} {}

    explicit Matrix(Matrix22<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0), ColumnT(0, 0, 1)} {}
    explicit Matrix(Matrix23<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(0, 0, 1)} {}
    explicit Matrix(Matrix34<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2])} {}
    explicit Matrix(Matrix44<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2])} {}

    template<class U> explicit Matrix(Matrix33<U> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2])} {}

    ColumnT& operator[](int _index) { return cols[_index]; }
    const ColumnT& operator[](int _index) const { return cols[_index]; }
    Matrix& operator*=(Matrix _m) { return *this = (*this * _m); }
    const T* ptr() const { return &cols[0].x; }

    private: Vector<3, T> cols[3];
};


template<class T> struct Matrix<3, 4, T> {
    Matrix() : cols{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}} {}
    Matrix(const Matrix& _m) : cols{_m[0], _m[1], _m[2]} {}
    using ColumnT = Vector<4, T>;

    explicit Matrix(T _s) : cols{{_s, 0, 0, 0}, {0, _s, 0, 0}, {0, 0, _s, 0}} {}
    explicit Matrix(ColumnT _x, ColumnT _y, ColumnT _z) : cols{_x, _y, _z} {}

    explicit Matrix(Matrix22<T> _m) : cols{ColumnT(_m[0], 0, 0), ColumnT(_m[1], 0, 0), ColumnT(0, 0, 1, 0)} {}
    explicit Matrix(Matrix23<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0), ColumnT(0, 0, 1, 0)} {}
    explicit Matrix(Matrix33<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0), ColumnT(_m[2], 0)} {}
    explicit Matrix(Matrix44<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2])} {}

    template<class U> explicit Matrix(Matrix34<U> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2])} {}

    ColumnT& operator[](int _index) { return cols[_index]; }
    const ColumnT& operator[](int _index) const { return cols[_index]; }
    //Matrix& operator*=(Matrix _m) { return *this = (*this * _m); }
    const T* ptr() const { return &cols[0].x; }

    private: Vector<4, T> cols[3];
};


template<class T> struct Matrix<4, 4, T> {
    Matrix() : cols{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {};
    Matrix(const Matrix& _m) : cols{_m[0], _m[1], _m[2], _m[3]} {}
    using ColumnT = Vector<4, T>;

    explicit Matrix(T _s) : cols{{_s, 0, 0, 0}, {0, _s, 0, 0}, {0, 0, _s, 0}, {0, 0, 0, _s}} {}
    explicit Matrix(ColumnT _x, ColumnT _y, ColumnT _z, ColumnT _w) : cols{_x, _y, _z, _w} {}

    explicit Matrix(Matrix22<T> _m) : cols{ColumnT(_m[0], 0, 0), ColumnT(_m[1], 0, 0), ColumnT(0, 0, 1, 0), ColumnT(0, 0, 0, 1)} {}
    explicit Matrix(Matrix23<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0), ColumnT(0, 0, 1, 0), ColumnT(0, 0, 0, 1)} {}
    explicit Matrix(Matrix33<T> _m) : cols{ColumnT(_m[0], 0), ColumnT(_m[1], 0), ColumnT(_m[2], 0), ColumnT(0, 0, 0, 1)} {}
    explicit Matrix(Matrix34<T> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2]), ColumnT(0, 0, 0, 1)} {}

    template<class U> explicit Matrix(Matrix44<U> _m) : cols{ColumnT(_m[0]), ColumnT(_m[1]), ColumnT(_m[2]), ColumnT(_m[3])} {}

    ColumnT& operator[](int _index) { return cols[_index]; }
    const ColumnT& operator[](int _index) const { return cols[_index]; }
    Matrix& operator*=(Matrix _m) { return *this = (*this * _m); }
    const T* ptr() const { return &cols[0].x; }

    private: Vector<4, T> cols[4];
};


// Multiply Matrix Matrix22 /////
template<class T> inline Matrix22<T> operator*(Matrix22<T> _a, Matrix22<T> _b) {
    return Matrix33<T>({_a[0]*_b[0][0] + _a[1]*_b[0][1]},
                       {_a[0]*_b[1][0] + _a[1]*_b[1][1]});
}

// Multiply Matrix Matrix33 /////
template<class T> inline Matrix33<T> operator*(Matrix33<T> _a, Matrix33<T> _b) {
    return Matrix33<T>({_a[0]*_b[0][0] + _a[1]*_b[0][1] + _a[2]*_b[0][2]},
                       {_a[0]*_b[1][0] + _a[1]*_b[1][1] + _a[2]*_b[1][2]},
                       {_a[0]*_b[2][0] + _a[1]*_b[2][1] + _a[2]*_b[2][2]});
}

// Multiply Matrix Matrix44 /////
template<class T> inline Matrix44<T> operator*(Matrix44<T> _a, Matrix44<T> _b) {
    return Matrix44<T>({_a[0]*_b[0][0] + _a[1]*_b[0][1] + _a[2]*_b[0][2] + _a[3]*_b[0][3]},
                       {_a[0]*_b[1][0] + _a[1]*_b[1][1] + _a[2]*_b[1][2] + _a[3]*_b[1][3]},
                       {_a[0]*_b[2][0] + _a[1]*_b[2][1] + _a[2]*_b[2][2] + _a[3]*_b[2][3]},
                       {_a[0]*_b[3][0] + _a[1]*_b[3][1] + _a[2]*_b[3][2] + _a[3]*_b[3][3]});
}

// Multiply Vector Matrix22 /////
template<class T> inline Vector3<T> operator*(Matrix22<T> _m, Vector2<T> _v) {
    return Vector2<T>(_m[0][0]*_v[0] + _m[1][0]*_v[1],
                      _m[0][1]*_v[0] + _m[1][1]*_v[1]);
}

// Multiply Vector Matrix33 /////
template<class T> inline Vector3<T> operator*(Matrix33<T> _m, Vector3<T> _v) {
    return Vector3<T>(_m[0][0]*_v[0] + _m[1][0]*_v[1] + _m[2][0]*_v[2],
                      _m[0][1]*_v[0] + _m[1][1]*_v[1] + _m[2][1]*_v[2],
                      _m[0][2]*_v[0] + _m[1][2]*_v[1] + _m[2][2]*_v[2]);
}

// Multiply Vector Matrix44 /////
template<class T> inline Vector4<T> operator*(Matrix44<T> _m, Vector4<T> _v) {
    return Vector4<T>(_m[0][0]*_v[0] + _m[1][0]*_v[1] + _m[2][0]*_v[2] + _m[3][0]*_v[3],
                      _m[0][1]*_v[0] + _m[1][1]*_v[1] + _m[2][1]*_v[2] + _m[3][1]*_v[3],
                      _m[0][2]*_v[0] + _m[1][2]*_v[1] + _m[2][2]*_v[2] + _m[3][2]*_v[3],
                      _m[0][3]*_v[0] + _m[1][3]*_v[1] + _m[2][3]*_v[2] + _m[3][3]*_v[3]);
}


namespace maths {

// Transpose Matrix33 /////
template<class T> inline Matrix33<T> transpose(Matrix33<T> _m) {
    return Matrix33<T>({_m[0][0], _m[1][0], _m[2][0]},
                       {_m[0][1], _m[1][1], _m[2][1]},
                       {_m[0][2], _m[1][2], _m[2][2]});
}

// Transpose Matrix44 /////
template<class T> inline Matrix44<T> transpose(Matrix44<T> _m) {
    return Matrix44<T>({_m[0][0], _m[1][0], _m[2][0], _m[3][0]},
                       {_m[0][1], _m[1][1], _m[2][1], _m[3][1]},
                       {_m[0][2], _m[1][2], _m[2][2], _m[3][2]},
                       {_m[0][3], _m[1][3], _m[2][3], _m[3][3]});
}

// Determinant Matrix33 /////
template<class T> inline T determinant(Matrix33<T> _m) {
    return _m[0][0] * (_m[1][1] * _m[2][2] - _m[2][1] * _m[1][2]) -
           _m[1][0] * (_m[0][1] * _m[2][2] - _m[2][1] * _m[0][2]) +
           _m[2][0] * (_m[0][1] * _m[1][2] - _m[1][1] * _m[0][2]);
}

// Determinant Matrix44 /////
template<class T> inline T determinant(Matrix44<T> _m) {
    T fac0 = _m[2][2] * _m[3][3] - _m[3][2] * _m[2][3];
    T fac1 = _m[2][1] * _m[3][3] - _m[3][1] * _m[2][3];
    T fac2 = _m[2][1] * _m[3][2] - _m[3][1] * _m[2][2];
    T fac3 = _m[2][0] * _m[3][3] - _m[3][0] * _m[2][3];
    T fac4 = _m[2][0] * _m[3][2] - _m[3][0] * _m[2][2];
    T fac5 = _m[2][0] * _m[3][1] - _m[3][0] * _m[2][1];

    T coef0 = +(_m[1][1] * fac0 - _m[1][2] * fac1 + _m[1][3] * fac2);
    T coef1 = -(_m[1][0] * fac0 - _m[1][2] * fac3 + _m[1][3] * fac4);
    T coef2 = +(_m[1][0] * fac1 - _m[1][1] * fac3 + _m[1][3] * fac5);
    T coef3 = -(_m[1][0] * fac2 - _m[1][1] * fac4 + _m[1][2] * fac5);

    return _m[0][0]*coef0 + _m[0][1]*coef1 + _m[0][2]*coef2 + _m[0][3]*coef3;
}

// Inverse Matrix33 /////
template<class T> inline Matrix33<T> inverse(Matrix33<T> _m) {
    T oneOverDet = T(1.0) / determinant(_m);

    return Matrix33<T>({+(_m[1][1] * _m[2][2] - _m[2][1] * _m[1][2]) * oneOverDet,
                        -(_m[0][1] * _m[2][2] - _m[2][1] * _m[0][2]) * oneOverDet,
                        +(_m[0][1] * _m[1][2] - _m[1][1] * _m[0][2]) * oneOverDet},
                       {-(_m[1][0] * _m[2][2] - _m[2][0] * _m[1][2]) * oneOverDet,
                        +(_m[0][0] * _m[2][2] - _m[2][0] * _m[0][2]) * oneOverDet,
                        -(_m[0][0] * _m[1][2] - _m[1][0] * _m[0][2]) * oneOverDet},
                       {+(_m[1][0] * _m[2][1] - _m[2][0] * _m[1][1]) * oneOverDet,
                        -(_m[0][0] * _m[2][1] - _m[2][0] * _m[0][1]) * oneOverDet,
                        +(_m[0][0] * _m[1][1] - _m[1][0] * _m[0][1]) * oneOverDet});
}

// Inverse Matrix44 /////
template<class T> inline Matrix44<T> inverse(Matrix44<T> _m) {
    T coef00 = _m[2][2] * _m[3][3] - _m[3][2] * _m[2][3];
    T coef02 = _m[1][2] * _m[3][3] - _m[3][2] * _m[1][3];
    T coef03 = _m[1][2] * _m[2][3] - _m[2][2] * _m[1][3];
    T coef04 = _m[2][1] * _m[3][3] - _m[3][1] * _m[2][3];
    T coef06 = _m[1][1] * _m[3][3] - _m[3][1] * _m[1][3];
    T coef07 = _m[1][1] * _m[2][3] - _m[2][1] * _m[1][3];

    T coef08 = _m[2][1] * _m[3][2] - _m[3][1] * _m[2][2];
    T coef10 = _m[1][1] * _m[3][2] - _m[3][1] * _m[1][2];
    T coef11 = _m[1][1] * _m[2][2] - _m[2][1] * _m[1][2];
    T coef12 = _m[2][0] * _m[3][3] - _m[3][0] * _m[2][3];

    T coef14 = _m[1][0] * _m[3][3] - _m[3][0] * _m[1][3];
    T coef15 = _m[1][0] * _m[2][3] - _m[2][0] * _m[1][3];
    T coef16 = _m[2][0] * _m[3][2] - _m[3][0] * _m[2][2];
    T coef18 = _m[1][0] * _m[3][2] - _m[3][0] * _m[1][2];

    T coef19 = _m[1][0] * _m[2][2] - _m[2][0] * _m[1][2];
    T coef20 = _m[2][0] * _m[3][1] - _m[3][0] * _m[2][1];
    T coef22 = _m[1][0] * _m[3][1] - _m[3][0] * _m[1][1];
    T coef23 = _m[1][0] * _m[2][1] - _m[2][0] * _m[1][1];

    Vector4<T> fac0(coef00, coef00, coef02, coef03);
    Vector4<T> fac1(coef04, coef04, coef06, coef07);
    Vector4<T> fac2(coef08, coef08, coef10, coef11);
    Vector4<T> fac3(coef12, coef12, coef14, coef15);
    Vector4<T> fac4(coef16, coef16, coef18, coef19);
    Vector4<T> fac5(coef20, coef20, coef22, coef23);

    Vector4<T> vec0(_m[1][0], _m[0][0], _m[0][0], _m[0][0]);
    Vector4<T> vec1(_m[1][1], _m[0][1], _m[0][1], _m[0][1]);
    Vector4<T> vec2(_m[1][2], _m[0][2], _m[0][2], _m[0][2]);
    Vector4<T> vec3(_m[1][3], _m[0][3], _m[0][3], _m[0][3]);

    Vector4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
    Vector4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
    Vector4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
    Vector4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

    Vector4<T> signA(+1, -1, +1, -1); Vector4<T> signB(-1, +1, -1, +1);
    Matrix44<T> invMat(inv0*signA, inv1*signB, inv2*signA, inv3*signB);

    T rep = T(1.0) / (_m[0][0]*invMat[0][0] + _m[0][1]*invMat[1][0]
                    + _m[0][2]*invMat[2][0] + _m[0][3]*invMat[3][0]);

    return Matrix44<T>(invMat[0]*rep, invMat[1]*rep, invMat[2]*rep, invMat[3]*rep);
}

// Scale /////
template<class T> inline Matrix44<T> scale(Matrix44<T> _m, Vector3<T> _v) {
    //return Matrix44<T>(_m[0]*_v.x, _m[1]*_v.y, _m[2]*_v.z, _m[3]);
    return Matrix44<T>(_m[0]*_v.x, _m[1]*_v.y, _m[2]*_v.z, _m[3]);
}

// Translate /////
template<class T> inline Matrix44<T> translate(Matrix44<T> _m, Vector3<T> _v) {
    return Matrix44<T>(_m[0], _m[1], _m[2], _m[0]*_v.x + _m[1]*_v.y + _m[2]*_v.z + _m[3]);
}

// Rotate /////
template<class T> inline Matrix44<T> rotate(Matrix44<T> _m, Vector3<T> _axis, T _angle) {
    T a = radians(_angle), s = std::sin(a), c = std::cos(a);
    Vector3<T> axis(normalize(_axis)), temp(axis * (T(1.0) - c));

    return Matrix44<T>(_m[0]*(c+temp.x*axis.x) + _m[1]*(temp.x*axis.y+s*axis.z) + _m[2]*(temp.x*axis.z-s*axis.y),
                       _m[0]*(temp.y*axis.x-s*axis.z) + _m[1]*(c+temp.y*axis.y) + _m[2]*(temp.y*axis.z+s*axis.x),
                       _m[0]*(temp.z*axis.x+s*axis.y) + _m[1]*(temp.z*axis.y-s*axis.x) + _m[2]*(c+temp.z*axis.z),
                       _m[3]);
}

// Look At /////
template<class T> inline Matrix44<T> look_at(Vector3<T> _eye, Vector3<T> _centre, Vector3<T> _up) {
    Vector3<T> f = normalize(_centre - _eye), s = normalize(cross(f, _up)), u = cross(s, f);
    return Matrix44<T>({s.x, u.x, -f.x, 0}, {s.y, u.y, -f.y, 0}, {s.z, u.z, -f.z, 0},
                       {-dot(s, _eye), -dot(u, _eye), dot(f, _eye), 1});
}

// Perspective /////
template<class T> inline Matrix44<T> perspective(T _fov, T _aspect, T _near, T _far) {
    T tanHalfFov = std::tan(_fov / T(2.0));
    T m00 = T(1.0) / (_aspect * tanHalfFov); T m11 = T(1.0) / tanHalfFov;
    T m22 = -(_far+_near) / (_far-_near);    T m32 = -(T(2.0)*_far*_near) / (_far-_near);
    return Matrix44<T>({m00, 0, 0, 0}, {0, m11, 0, 0}, {0, 0, m22, -1}, {0, 0, m32, 0});
}

// Orthographic /////
template<class T> inline Matrix44<T> ortho(T _l, T _r, T _b, T _t, T _n, T _f) {
    T m00 = T(2.0) / (_r-_l);   T m11 = T(2.0) / (_t-_b);   T m22 = -T(2.0) / (_f-_n);
    T m30 = -(_r+_l) / (_r-_l); T m31 = -(_t+_b) / (_t-_b); T m32 = -(_f+_n) / (_f-_n);
    return Matrix44<T>({m00, 0, 0, 0}, {0, m11, 0, 0}, {0, 0, m22, 0}, {m30, m31, m32, 1});
}

}}

using Mat2F  = sq::Matrix<2, 2, float>;
using Mat23F = sq::Matrix<2, 3, float>;
using Mat3F  = sq::Matrix<3, 3, float>;
using Mat34F = sq::Matrix<3, 4, float>;
using Mat4F  = sq::Matrix<4, 4, float>;
