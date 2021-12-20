// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>

#include <fmt/format.h>

namespace sq { namespace maths {

//============================================================================//

template <int W, int H, class T> struct Matrix {};

template <class T> using Matrix33 = Matrix<3, 3, T>;
template <class T> using Matrix34 = Matrix<3, 4, T>;
template <class T> using Matrix44 = Matrix<4, 4, T>;

//============================================================================//

template <class T> struct Matrix<3, 3, T>
{
    static_assert(std::is_floating_point_v<T>);

    // Column Type Alias
    using Column = Vector<3, T>;

    // Default and Copy Constructors
    constexpr Matrix() : data { 1,0,0, 0,1,0, 0,0,1 } {}
    constexpr Matrix(const Matrix& m) : columns { m[0], m[1], m[2] } {}
    constexpr Matrix& operator=(const Matrix& m) = default;

    // Uninitialise Constructor
    constexpr Matrix(std::nullptr_t) {}

    // Scalar and Vector Constructors
    constexpr explicit Matrix(T s) : data { s,0,0, 0,s,0, 0,0,s } {}
    constexpr explicit Matrix(Column a, Column b, Column c) : columns { a, b, c } {}

    // Matrix Resize Constructors
    constexpr explicit Matrix(Matrix34<T> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}
    constexpr explicit Matrix(Matrix44<T> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}

    // Column Access Operators
    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }

    union { T data[3*3]; Column columns[3] { nullptr, nullptr, nullptr }; };
};

//============================================================================//

template <class T> struct Matrix<3, 4, T>
{
    static_assert(std::is_floating_point_v<T>);

    // Column Type Alias
    using Column = Vector<4, T>;

    // Default and Copy Constructors
    constexpr Matrix() : data { 1,0,0,0, 0,1,0,0, 0,0,1,0 } {}
    constexpr Matrix(const Matrix& m) : columns { m[0], m[1], m[2] } {}
    constexpr Matrix& operator=(const Matrix& m) = default;

    // Uninitialise Constructor
    constexpr Matrix(std::nullptr_t) {}

    // Scalar and Vector Constructors
    constexpr explicit Matrix(T s) : data { s,0,0,0, 0,s,0,0, 0,0,s,0 } {}
    constexpr explicit Matrix(Column a, Column b, Column c) : columns { a, b, c } {}

    // Matrix Resize Constructors
    constexpr explicit Matrix(Matrix33<T> m) : columns { Column(m[0], 0), Column(m[1], 0), Column(m[2], 0) } {}
    constexpr explicit Matrix(Matrix44<T> m) : columns { m[0], m[1], m[2] } {}

    // Column Access Operators
    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }

    union { T data[3*4]; Column columns[3] { nullptr, nullptr, nullptr }; };
};

//============================================================================//

template <class T> struct Matrix<4, 4, T>
{
    static_assert(std::is_floating_point_v<T>);

    // Column Type Alias
    using Column = Vector<4, T>;

    // Default and Copy Constructors
    constexpr Matrix() : data { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 } {}
    constexpr Matrix(const Matrix& m) : columns { m[0], m[1], m[2], m[3] } {}
    constexpr Matrix& operator=(const Matrix& m) = default;

    // Uninitialise Constructor
    constexpr Matrix(std::nullptr_t) {}

    // Scalar and Vector Constructors
    constexpr explicit Matrix(T s) : data { s,0,0,0, 0,s,0,0, 0,0,s,0, 0,0,0,s } {}
    constexpr explicit Matrix(Column a, Column b, Column c, Column d) : columns { a, b, c, d } {}

    // Matrix Resize Constructors
    constexpr explicit Matrix(Matrix33<T> m) : columns { Column(m[0], 0), Column(m[1], 0), Column(m[2], 0), Column(0,0,0,1) } {}
    constexpr explicit Matrix(Matrix34<T> m) : columns { m[0], m[1], m[2], Column(0,0,0,1) } {}

    // Column Access Operators
    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }

    union { T data[4*4]; Column columns[4] { nullptr, nullptr, nullptr, nullptr }; };
};

//============================================================================//

// equality, inequality (Matrix, Matrix) /////

template <int W, int H, class T> constexpr
bool operator==(Matrix<W, H, T> a, Matrix<W, H, T> b)
{
    for (int i = 0; i < W; ++i)
        for (int j = 0; j < H; ++j)
            if (a[i][j] != b[i][j])
                return false;
    return true;
}

template <int W, int H, class T> constexpr
bool operator!=(Matrix<W, H, T> a, Matrix<W, H, T> b)
{
    for (int i = 0; i < W; ++i)
        for (int j = 0; j < H; ++j)
            if (a[i][j] != b[i][j])
                return true;
    return false;
}

//============================================================================//

// Multiplication (Matrix, Matrix) /////

template <class T> constexpr
Matrix33<T> operator*(Matrix33<T> a, Matrix33<T> b)
{
    Vector3<T> colA = a[0]*b[0][0] + a[1]*b[0][1] + a[2]*b[0][2];
    Vector3<T> colB = a[0]*b[1][0] + a[1]*b[1][1] + a[2]*b[1][2];
    Vector3<T> colC = a[0]*b[2][0] + a[1]*b[2][1] + a[2]*b[2][2];
    return Matrix33<T> ( colA, colB, colC );
}

template <class T> constexpr
Matrix44<T> operator*(Matrix44<T> a, Matrix44<T> b)
{
    Vector4<T> colA = a[0]*b[0][0] + a[1]*b[0][1] + a[2]*b[0][2] + a[3]*b[0][3];
    Vector4<T> colB = a[0]*b[1][0] + a[1]*b[1][1] + a[2]*b[1][2] + a[3]*b[1][3];
    Vector4<T> colC = a[0]*b[2][0] + a[1]*b[2][1] + a[2]*b[2][2] + a[3]*b[2][3];
    Vector4<T> colD = a[0]*b[3][0] + a[1]*b[3][1] + a[2]*b[3][2] + a[3]*b[3][3];
    return Matrix44<T> ( colA, colB, colC, colD );
}

//============================================================================//

// Multiplication (Matrix, Vector) /////

template <class T> constexpr
Vector3<T> operator*(Matrix33<T> m, Vector3<T> v)
{
    T x = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2];
    T y = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2];
    T z = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2];
    return Vector3<T> { x, y, z };
}

template <class T> constexpr
Vector4<T> operator*(Matrix44<T> m, Vector4<T> v)
{
    T x = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]*v[3];
    T y = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]*v[3];
    T z = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]*v[3];
    T w = m[0][3]*v[0] + m[1][3]*v[1] + m[2][3]*v[2] + m[3][3]*v[3];
    return Vector4<T> { x, y, z, w };
}

//============================================================================//

/// Compute the transpose of a Matrix33.
template <class T> constexpr
Matrix33<T> transpose(Matrix33<T> m)
{
    Vector3<T> colA { m[0][0], m[1][0], m[2][0] };
    Vector3<T> colB { m[0][1], m[1][1], m[2][1] };
    Vector3<T> colC { m[0][2], m[1][2], m[2][2] };
    return Matrix33<T> ( colA, colB, colC );
}

/// Compute the transpose of a Matrix44.
template <class T> constexpr
Matrix44<T> transpose(Matrix44<T> m)
{
    Vector4<T> colA { m[0][0], m[1][0], m[2][0], m[3][0] };
    Vector4<T> colB { m[0][1], m[1][1], m[2][1], m[3][1] };
    Vector4<T> colC { m[0][2], m[1][2], m[2][2], m[3][2] };
    Vector4<T> colD { m[0][3], m[1][3], m[2][3], m[3][3] };
    return Matrix44<T> ( colA, colB, colC, colD );
}

//============================================================================//

/// Compute the determinant of a Matrix33.
template <class T> inline
T determinant(Matrix33<T> m)
{
    T valueA = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]);
    T valueB = m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]);
    T valueC = m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    return valueA - valueB + valueC;
}

/// Compute the determinant of a Matrix44.
template <class T> inline
T determinant(Matrix44<T> m)
{
    T fac0 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    T fac1 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    T fac2 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    T fac3 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    T fac4 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    T fac5 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

    T ceX = +(m[1][1] * fac0 - m[1][2] * fac1 + m[1][3] * fac2);
    T ceY = -(m[1][0] * fac0 - m[1][2] * fac3 + m[1][3] * fac4);
    T ceZ = +(m[1][0] * fac1 - m[1][1] * fac3 + m[1][3] * fac5);
    T ceW = -(m[1][0] * fac2 - m[1][1] * fac4 + m[1][2] * fac5);

    return maths::dot(m[0], Vector4<T>(ceX, ceY, ceZ, ceW));
}

//============================================================================//

/// Compute the inverse of a Matrix33.
template <class T> inline
Matrix33<T> inverse(Matrix33<T> m)
{
    T ax = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]);
    T ay = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]);
    T az = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    T bx = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]);
    T by = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]);
    T bz = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]);
    T cx = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]);
    T cy = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]);
    T cz = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]);

    T invDet = T(1.0) / maths::determinant(m);
    auto colA = Vector3<T>(ax, ay, az) * invDet;
    auto colB = Vector3<T>(bx, by, bz) * invDet;
    auto colC = Vector3<T>(cx, cy, cz) * invDet;
    return Matrix33<T> ( colA, colB, colC );
}

/// Compute the inverse of a Matrix44.
template <class T> inline
Matrix44<T> inverse(Matrix44<T> m)
{
    T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

    T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];

    T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];

    T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

    Vector4<T> fac0 { coef00, coef00, coef02, coef03 };
    Vector4<T> fac1 { coef04, coef04, coef06, coef07 };
    Vector4<T> fac2 { coef08, coef08, coef10, coef11 };
    Vector4<T> fac3 { coef12, coef12, coef14, coef15 };
    Vector4<T> fac4 { coef16, coef16, coef18, coef19 };
    Vector4<T> fac5 { coef20, coef20, coef22, coef23 };

    Vector4<T> vec0 { m[1][0], m[0][0], m[0][0], m[0][0] };
    Vector4<T> vec1 { m[1][1], m[0][1], m[0][1], m[0][1] };
    Vector4<T> vec2 { m[1][2], m[0][2], m[0][2], m[0][2] };
    Vector4<T> vec3 { m[1][3], m[0][3], m[0][3], m[0][3] };

    Vector4<T> invA = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
    Vector4<T> invB = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
    Vector4<T> invC = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
    Vector4<T> invD = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

    invA = invA * Vector4<T>(+1.0, -1.0, +1.0, -1.0);
    invB = invB * Vector4<T>(-1.0, +1.0, -1.0, +1.0);
    invC = invC * Vector4<T>(+1.0, -1.0, +1.0, -1.0);
    invD = invD * Vector4<T>(-1.0, +1.0, -1.0, +1.0);

    Vector4<T> repA { m[0][0], m[0][1], m[0][2], m[0][3] };
    Vector4<T> repB { invA[0], invB[0], invC[0], invD[0] };
    T r = T(1.0) / maths::dot(repA, repB);

    return Matrix44<T> ( invA*r, invB*r, invC*r, invD*r );
}

//============================================================================//

/// Compute the inverse of an affine Matrix44.
template <class T> inline
Matrix44<T> affine_inverse(Matrix44<T> m)
{
    Matrix33<T> inv33 = maths::inverse(Matrix33<T>(m));

    Vector4<T> colA { inv33[0], T(0.0) };
    Vector4<T> colB { inv33[1], T(0.0) };
    Vector4<T> colC { inv33[2], T(0.0) };

    Vector4<T> colD { -(inv33 * Vector3<T>(m[3])), T(1.0) };

    return Matrix44<T> ( colA, colB, colC, colD );
}

//============================================================================//

/// Compute the normal matrix of a model view matrix.
template <class T> inline
Matrix33<T> normal_matrix(Matrix44<T> modelView)
{
    return maths::transpose(maths::inverse(Matrix33<T>(modelView)));
}

//============================================================================//

} // namespace maths

template <int W, int H, class T>
constexpr const char* type_to_string(maths::Matrix<W, H, T>)
{
    if constexpr (W == 3 && H == 3 && std::is_same_v<T, float>) return "Mat3F";
    if constexpr (W == 3 && H == 4 && std::is_same_v<T, float>) return "Mat34F";
    if constexpr (W == 4 && H == 4 && std::is_same_v<T, float>) return "Mat4F";
    return "Matrix<W, H, T>";
}

//============================================================================//

namespace detail {

template <class T>
struct MatrixTraits : std::false_type {};

template <class T>
struct MatrixTraits<const T> : MatrixTraits<T> {};

template <int W, int H, class T>
struct MatrixTraits<maths::Matrix<W, H, T>> : std::true_type
{
   static constexpr int width = W;
   static constexpr int height = H;
   using type = T;
};

template <class T>
constexpr bool is_matrix_v = MatrixTraits<T>::value;

template <class T>
constexpr int matrix_width_v = MatrixTraits<T>::width;

template <class T>
constexpr int matrix_height_v = MatrixTraits<T>::height;

template <class T>
using matrix_type_t = typename MatrixTraits<T>::type;

}} // namespace sq::detail

//============================================================================//

template <int W, int H, class T>
struct fmt::formatter<sq::maths::Matrix<W, H, T>> : fmt::formatter<T>
{
    template <class FormatContext>
    auto format(const sq::maths::Matrix<W, H, T>& mat, FormatContext& ctx)
    {
        // this func does support W/H of 2 in case I ever need 2D matrices
        fmt::format_to(ctx.out(), "{}((", sq::type_to_string(sq::maths::Matrix<W, H, T>()));
        formatter<T>::format(mat[0][0], ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(mat[0][1], ctx);
        if constexpr (H >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (H >= 3) formatter<T>::format(mat[0][2], ctx);
        if constexpr (H == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (H == 4) formatter<T>::format(mat[0][3], ctx);
        fmt::format_to(ctx.out(), "), (");
        formatter<T>::format(mat[1][0], ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(mat[1][1], ctx);
        if constexpr (H >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (H >= 3) formatter<T>::format(mat[1][2], ctx);
        if constexpr (H == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (H == 4) formatter<T>::format(mat[1][3], ctx);
        if constexpr (W >= 3) fmt::format_to(ctx.out(), "), (");
        if constexpr (W >= 3) formatter<T>::format(mat[2][0], ctx);
        if constexpr (W >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (W >= 3) formatter<T>::format(mat[2][1], ctx);
        if constexpr (W >= 3 && H >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (W >= 3 && H >= 3) formatter<T>::format(mat[2][2], ctx);
        if constexpr (W >= 3 && H == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (W >= 3 && H == 4) formatter<T>::format(mat[2][3], ctx);
        if constexpr (W == 4) fmt::format_to(ctx.out(), "), (");
        if constexpr (W == 4) formatter<T>::format(mat[3][0], ctx);
        if constexpr (W == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (W == 4) formatter<T>::format(mat[3][1], ctx);
        if constexpr (W == 4 && H >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (W == 4 && H >= 3) formatter<T>::format(mat[3][2], ctx);
        if constexpr (W == 4 && H == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (W == 4 && H == 4) formatter<T>::format(mat[3][3], ctx);
        return fmt::format_to(ctx.out(), "))");
    }
};
