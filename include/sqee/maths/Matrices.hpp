// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq::maths { //########################################################

template <int W, int H, class T> struct Matrix;

template <class T> using Matrix33 = Matrix<3, 3, T>;
template <class T> using Matrix34 = Matrix<3, 4, T>;
template <class T> using Matrix44 = Matrix<4, 4, T>;

/// Concept that matches any kind of matrix.
template <class MatrixWHT>
concept AnyMatrix = requires (MatrixWHT m)
{
    []<int W, int H, class T>(Matrix<W, H, T>&){}(m);
};

//==============================================================================

template <class T> struct Matrix<3, 3, T>
{
    static_assert(std::is_floating_point_v<T>);

    using Column = Vector<3, T>;

    Column columns[3];

    constexpr Matrix() : columns { {1,0,0}, {0,1,0}, {0,0,1} } {}
    constexpr Matrix(Column a, Column b, Column c) : columns { a, b, c } {}

    constexpr explicit Matrix(T s) : columns { {s,0,0}, {0,s,0}, {0,0,s} } {}

    constexpr Matrix(const Matrix& m) = default;
    constexpr Matrix& operator=(const Matrix& m) = default;

    /// Construct without initialisation.
    Matrix(std::nullptr_t) {}

    /// Construct from a matrix of a different scalar type.
    template <class U>
    constexpr explicit Matrix(Matrix33<U> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}

    constexpr explicit Matrix(Matrix34<T> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}
    constexpr explicit Matrix(Matrix44<T> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}

    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }
};

template <class T> Matrix(Vector3<T>, Vector3<T>, Vector3<T>) -> Matrix<3, 3, T>;

//==============================================================================

template <class T> struct Matrix<3, 4, T>
{
    static_assert(std::is_floating_point_v<T>);

    using Column = Vector<4, T>;

    Column columns[3];

    constexpr Matrix() : columns { {1,0,0,0}, {0,1,0,0}, {0,0,1,0} } {}
    constexpr Matrix(Column a, Column b, Column c) : columns { a, b, c } {}

    constexpr explicit Matrix(T s) : columns { {s,0,0,0}, {0,s,0,0}, {0,0,s,0} } {}

    constexpr Matrix(const Matrix& m) = default;
    constexpr Matrix& operator=(const Matrix& m) = default;

    /// Construct without initialisation.
    Matrix(std::nullptr_t) {}

    /// Construct from a matrix of a different scalar type.
    template <class U>
    constexpr explicit Matrix(Matrix34<U> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]) } {}

    constexpr explicit Matrix(Matrix33<T> m) : columns { Column(m[0], 0), Column(m[1], 0), Column(m[2], 0) } {}
    constexpr explicit Matrix(Matrix44<T> m) : columns { m[0], m[1], m[2] } {}

    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }
};

template <class T> Matrix(Vector4<T>, Vector4<T>, Vector4<T>) -> Matrix<3, 4, T>;

//==============================================================================

template <class T> struct Matrix<4, 4, T>
{
    static_assert(std::is_floating_point_v<T>);

    using Column = Vector<4, T>;

    Column columns[4];

    constexpr Matrix() : columns { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} } {}
    constexpr Matrix(Column a, Column b, Column c, Column d) : columns { a, b, c, d } {}

    constexpr explicit Matrix(T s) : columns { {s,0,0,0}, {0,s,0,0}, {0,0,s,0}, {0,0,0,s} } {}

    constexpr Matrix(const Matrix& m) = default;
    constexpr Matrix& operator=(const Matrix& m) = default;

    /// Construct without initialisation.
    Matrix(std::nullptr_t) {}

    /// Construct from a matrix of a different scalar type.
    template <class U>
    constexpr explicit Matrix(Matrix44<U> m) : columns { Column(m[0]), Column(m[1]), Column(m[2]), Column(m[3]) } {}

    constexpr explicit Matrix(Matrix33<T> m) : columns { Column(m[0], 0), Column(m[1], 0), Column(m[2], 0), {0,0,0,1} } {}
    constexpr explicit Matrix(Matrix34<T> m) : columns { m[0], m[1], m[2], {0,0,0,1} } {}

    constexpr Column& operator[](int index) { return columns[index]; }
    constexpr const Column& operator[](int index) const { return columns[index]; }
};

template <class T> Matrix(Vector4<T>, Vector4<T>, Vector4<T>, Vector4<T>) -> Matrix<4, 4, T>;

//==============================================================================

template <int W, int H, class T>
constexpr bool operator==(Matrix<W, H, T> a, Matrix<W, H, T> b)
{
    for (int i = 0; i < W; ++i)
        for (int j = 0; j < H; ++j)
            if (a[i][j] != b[i][j]) return false;
    return true;
}

//==============================================================================

template <class T>
constexpr Matrix33<T> operator*(Matrix33<T> a, Matrix33<T> b)
{
    Vector colA = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2];
    Vector colB = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2];
    Vector colC = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2];
    return { colA, colB, colC };
}

template <class T>
constexpr Matrix44<T> operator*(Matrix44<T> a, Matrix44<T> b)
{
    Vector colA = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2] + a[3] * b[0][3];
    Vector colB = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2] + a[3] * b[1][3];
    Vector colC = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2] + a[3] * b[2][3];
    Vector colD = a[0] * b[3][0] + a[1] * b[3][1] + a[2] * b[3][2] + a[3] * b[3][3];
    return { colA, colB, colC, colD };
}

//==============================================================================

template <class T>
constexpr Vector3<T> operator*(Matrix33<T> m, Vector3<T> v)
{
    T x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
    T y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
    T z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];
    return { x, y, z };
}

template <class T>
constexpr Vector4<T> operator*(Matrix44<T> m, Vector4<T> v)
{
    T x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    T y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    T z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    T w = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
    return { x, y, z, w };
}

//==============================================================================

/// Transpose a 3x3 Matrix.
template <class T>
constexpr Matrix33<T> transpose(Matrix33<T> m)
{
    Vector colA { m[0][0], m[1][0], m[2][0] };
    Vector colB { m[0][1], m[1][1], m[2][1] };
    Vector colC { m[0][2], m[1][2], m[2][2] };
    return { colA, colB, colC };
}

/// Transpose a 4x4 Matrix.
template <class T>
constexpr Matrix44<T> transpose(Matrix44<T> m)
{
    Vector colA { m[0][0], m[1][0], m[2][0], m[3][0] };
    Vector colB { m[0][1], m[1][1], m[2][1], m[3][1] };
    Vector colC { m[0][2], m[1][2], m[2][2], m[3][2] };
    Vector colD { m[0][3], m[1][3], m[2][3], m[3][3] };
    return { colA, colB, colC, colD };
}

//==============================================================================

/// Compute the determinant of a 3x3 Matrix.
template <class T>
inline T determinant(Matrix33<T> m)
{
    T valueA = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]);
    T valueB = m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]);
    T valueC = m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

    return valueA - valueB + valueC;
}

/// Compute the determinant of a 4x4 Matrix.
template <class T>
inline T determinant(Matrix44<T> m)
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

    return maths::dot(m[0], Vector(ceX, ceY, ceZ, ceW));
}

//==============================================================================

/// Compute the inverse of a 3x3 Matrix.
template <class T>
inline Matrix33<T> inverse(Matrix33<T> m)
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

    Vector colA = Vector(ax, ay, az) * invDet;
    Vector colB = Vector(bx, by, bz) * invDet;
    Vector colC = Vector(cx, cy, cz) * invDet;

    return { colA, colB, colC };
}

/// Compute the inverse of a 4x4 Matrix.
template <class T>
inline Matrix44<T> inverse(Matrix44<T> m)
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

    Vector fac0 { coef00, coef00, coef02, coef03 };
    Vector fac1 { coef04, coef04, coef06, coef07 };
    Vector fac2 { coef08, coef08, coef10, coef11 };
    Vector fac3 { coef12, coef12, coef14, coef15 };
    Vector fac4 { coef16, coef16, coef18, coef19 };
    Vector fac5 { coef20, coef20, coef22, coef23 };

    Vector vec0 { m[1][0], m[0][0], m[0][0], m[0][0] };
    Vector vec1 { m[1][1], m[0][1], m[0][1], m[0][1] };
    Vector vec2 { m[1][2], m[0][2], m[0][2], m[0][2] };
    Vector vec3 { m[1][3], m[0][3], m[0][3], m[0][3] };

    Vector invA = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
    Vector invB = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
    Vector invC = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
    Vector invD = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

    invA = invA * Vector4<T>(+1, -1, +1, -1);
    invB = invB * Vector4<T>(-1, +1, -1, +1);
    invC = invC * Vector4<T>(+1, -1, +1, -1);
    invD = invD * Vector4<T>(-1, +1, -1, +1);

    Vector repA { m[0][0], m[0][1], m[0][2], m[0][3] };
    Vector repB { invA[0], invB[0], invC[0], invD[0] };

    T r = T(1.0) / maths::dot(repA, repB);

    return { invA*r, invB*r, invC*r, invD*r };
}

//==============================================================================

/// Compute the inverse of an affine transform Matrix.
template <class T>
inline Matrix44<T> affine_inverse(Matrix44<T> m)
{
    Matrix inv33 = maths::inverse(Matrix33<T>(m));

    Vector colA { inv33[0], T(0.0) };
    Vector colB { inv33[1], T(0.0) };
    Vector colC { inv33[2], T(0.0) };
    Vector colD { -(inv33 * Vector3<T>(m[3])), T(1.0) };

    return { colA, colB, colC, colD };
}

/// Compute the normal matrix of a model view matrix.
template <class T>
inline Matrix33<T> normal_matrix(Matrix44<T> modelView)
{
    return maths::transpose(maths::inverse(Matrix33<T>(modelView)));
}

} // namespace sq::maths #######################################################

#include <sqee/core/TypeNames.hpp>

#include <fmt/format.h>

template <int W, int H, class T>
struct fmt::formatter<sq::maths::Matrix<W, H, T>>
{
    fmt::formatter<T> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    FormatContext::iterator format(const sq::maths::Matrix<W, H, T>& mat, FormatContext& ctx) const
    {
        // note that this does support W/H of 2 in case I ever need 2D matrices
        ctx.advance_to(fmt::detail::write(ctx.out(), sq::type_name_v<sq::maths::Matrix<W, H, T>>.c_str()));
        ctx.advance_to(fmt::detail::write(ctx.out(), "(("));
        ctx.advance_to(base.format(mat[0][0], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(mat[0][1], ctx));
        if constexpr (H >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (H >= 3) ctx.advance_to(base.format(mat[0][2], ctx));
        if constexpr (H == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (H == 4) ctx.advance_to(base.format(mat[0][3], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), "), ("));
        ctx.advance_to(base.format(mat[1][0], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(mat[1][1], ctx));
        if constexpr (H >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (H >= 3) ctx.advance_to(base.format(mat[1][2], ctx));
        if constexpr (H == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (H == 4) ctx.advance_to(base.format(mat[1][3], ctx));
        if constexpr (W >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), "), ("));
        if constexpr (W >= 3) ctx.advance_to(base.format(mat[2][0], ctx));
        if constexpr (W >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W >= 3) ctx.advance_to(base.format(mat[2][1], ctx));
        if constexpr (W >= 3 && H >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W >= 3 && H >= 3) ctx.advance_to(base.format(mat[2][2], ctx));
        if constexpr (W >= 3 && H == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W >= 3 && H == 4) ctx.advance_to(base.format(mat[2][3], ctx));
        if constexpr (W == 4) ctx.advance_to(fmt::detail::write(ctx.out(), "), ("));
        if constexpr (W == 4) ctx.advance_to(base.format(mat[3][0], ctx));
        if constexpr (W == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W == 4) ctx.advance_to(base.format(mat[3][1], ctx));
        if constexpr (W == 4 && H >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W == 4 && H >= 3) ctx.advance_to(base.format(mat[3][2], ctx));
        if constexpr (W == 4 && H == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (W == 4 && H == 4) ctx.advance_to(base.format(mat[3][3], ctx));
        return fmt::detail::write(ctx.out(), "))");
    }
};
