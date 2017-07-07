#pragma once

#include <limits>

#include <sqee/setup.hpp>

namespace sq::maths {

//============================================================================//

/// Minimum of two scalar arguments.
template <class T> constexpr T min(T a, T b)
{
    static_assert(std::is_arithmetic_v<T>);

    return a < b ? a : b;
}

/// Maximum of two scalar arguments.
template <class T> constexpr T max(T a, T b)
{
    static_assert(std::is_arithmetic_v<T>);

    return a > b ? a : b;
}

//============================================================================//

/// Minimum of three scalar arguments.
template <class T> constexpr T min(T a, T b, T c)
{
    static_assert(std::is_arithmetic_v<T>);

    return maths::min(maths::min(a, b), c);
}

/// Maximum of three scalar arguments.
template <class T> constexpr T max(T a, T b, T c)
{
    static_assert(std::is_arithmetic_v<T>);

    return maths::max(maths::max(a, b), c);
}

//============================================================================//

/// Minimum of four scalar arguments.
template <class T> constexpr T min(T a, T b, T c, T d)
{
    static_assert(std::is_arithmetic_v<T>);

    return maths::min(maths::min(maths::min(a, b), c), d);
}

/// Maximum of four scalar arguments.
template <class T> constexpr T max(T a, T b, T c, T d)
{
    static_assert(std::is_arithmetic_v<T>);

    return maths::max(maths::max(maths::max(a, b), c), d);
}

//============================================================================//

/// Clamp a scalar value within a range,
template <class T> constexpr T clamp(T value, T min, T max)
{
    static_assert(std::is_arithmetic_v<T>);

    return maths::min(maths::max(value, min), max);
}

//============================================================================//

/// Linearly interpolate between two scalars.
template <class T> constexpr T mix(T a, T b, T factor)
{
    static_assert(std::is_floating_point_v<T>);

    return a * (T(1.0) - factor) + b * factor;
}

//============================================================================//

/// Convert a scalar from cycles to radians.
template <class T> constexpr T radians(T cycles)
{
    static_assert(std::is_floating_point_v<T>);

    return cycles * T(2.0 * 3.14159265358979323846);
}

/// Convert a scalar from radians to cycles.
template <class T> constexpr T cycles(T radians)
{
    static_assert(std::is_floating_point_v<T>);

    return radians * T(0.5 / 3.14159265358979323846);
}

//============================================================================//

} // namespace sq::maths
