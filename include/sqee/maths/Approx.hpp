// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <fmt/format.h>

#include <limits>
#include <type_traits>

namespace sq::maths {

//============================================================================//

/// Simple templated version of a Catch's Approx class.
template <class T> struct Approx
{
    constexpr explicit Approx(T value) : value(value) {}
    constexpr Approx(const Approx& other) : value(other.value) {}

    static_assert (
        std::is_floating_point_v<T> || detail::is_vector_v<T> || detail::is_matrix_v<T> || detail::is_quaternion_v<T>,
        "Approx not implemented for this type"
    );

    const T value;
};

// deduction guide so we don't need a helper function
template<class T> Approx(T) -> Approx<T>;

//============================================================================//

// note that epsilon is just high enough for current unit tests to pass

constexpr bool operator==(float a, Approx<float> b)
{
    const float maxScale = maths::max(maths::abs(a), maths::abs(b.value));
    return maths::abs(a - b.value) <= std::numeric_limits<float>::epsilon() * 5.f * (1.f + maxScale);
}

constexpr bool operator==(double a, Approx<double> b)
{
    const double maxScale = maths::max(maths::abs(a), maths::abs(b.value));
    return maths::abs(a - b.value) <= std::numeric_limits<double>::epsilon() * 5.0 * (1.0 + maxScale);
}

template <int S, class T>
constexpr bool operator==(Vector<S, T> a, Approx<Vector<S, T>> b)
{
    bool result = true;
    for (int i = 0; i < S; ++i)
        result &= a[i] == Approx(b.value[i]);
    return result;
}

template <int H, int W, class T>
constexpr bool operator==(Matrix<H, W, T> a, Approx<Matrix<H, W, T>> b)
{
    bool result = true;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j)
            result &= a[i][j] == Approx(b.value[i][j]);
    return result;
}

template <class T>
constexpr bool operator==(Quaternion<T> a, Approx<Quaternion<T>> b)
{
    bool resultPos = true;
    for (int i = 0; i < 4; ++i)
        resultPos &= +a[i] == Approx(b.value[i]);
    bool resultNeg = true;
    for (int i = 0; i < 4; ++i)
        resultNeg &= -a[i] == Approx(b.value[i]);
    return resultPos || resultNeg;
}

// these can all be defined in terms of the same operator
template <class T> inline bool operator==(Approx<T> a, T b) { return operator==(b, a); }
template <class T> inline bool operator!=(T a, Approx<T> b) { return !operator==(a, b); }
template <class T> inline bool operator!=(Approx<T> a, T b) { return !operator==(b, a); }

//============================================================================//

} // namespace sq::maths

//============================================================================//

template <class Type>
struct fmt::formatter<sq::maths::Approx<Type>> : fmt::formatter<Type>
{
    template <class FormatContext>
    auto format(const sq::maths::Approx<Type>& approx, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "Approx(");
        formatter<Type>::format(approx.value, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};
