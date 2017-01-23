#pragma once

#include <limits>

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

template <class T> using if_numeric = std::enable_if_t<std::is_arithmetic<T>::value>;
template <class T> using if_float = std::enable_if_t<std::is_floating_point<T>::value>;
template <class T> using if_integer = std::enable_if_t<std::is_integral<T>::value>;

//============================================================================//

namespace maths {

//============================================================================//

// minimum, maximum (Scalars) /////

template <class T, if_numeric<T>...> constexpr T min(T a, T b) { return a < b ? a : b; }
template <class T, if_numeric<T>...> constexpr T max(T a, T b) { return a > b ? a : b; }

template <class T, if_numeric<T>...> constexpr T min(T a, T b, T c) { return min(min(a, b), c); }
template <class T, if_numeric<T>...> constexpr T max(T a, T b, T c) { return max(max(a, b), c); }

template <class T, if_numeric<T>...> constexpr T min(T a, T b, T c, T d) { return min(min(min(a, b), c), d); }
template <class T, if_numeric<T>...> constexpr T max(T a, T b, T c, T d) { return max(max(max(a, b), c), d); }

//============================================================================//

// clamp to range (Scalar) /////

template <class T, if_numeric<T>...>
constexpr T clamp(T value, T min, T max)
{
    return maths::min(maths::max(value, min), max);
}

//============================================================================//

// linear mix (Scalar) /////

template <class T, if_float<T>...>
constexpr T mix(T a, T b, T factor)
{
    return a * (T(1.0) - factor) + b * factor;
}

//============================================================================//

// convert between radians and cycles (Scalar) /////

template <class T, if_float<T>...> constexpr
T radians(T cycles) { return cycles * T(2.0 * 3.14159265358979323846); }

template <class T, if_float<T>...> constexpr
T cycles(T radians) { return radians * T(0.5 / 3.14159265358979323846); }

//============================================================================//

// to the power of two (Scalar) /////

template <class T, if_numeric<T>...>
constexpr T square(T value)
{
    return value * value;
}

//============================================================================//

}} // namespace sq::maths
