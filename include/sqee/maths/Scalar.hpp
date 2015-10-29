#pragma once

#include <limits>
#include <type_traits>

#include <sqee/setup.hpp>

namespace sq {

template<class T> using is_numeric = typename std::enable_if<std::is_arithmetic<T>::value>::type;
template<class T> using is_float = typename std::enable_if<std::is_floating_point<T>::value>::type;
template<class T> using is_integer = typename std::enable_if<std::is_integral<T>::value>::type;

namespace maths {

// Minimum / Maximum of 2 /////
template<class T, is_numeric<T>...>
inline T min(T _a, T _b) { return _a < _b ? _a : _b; }
template<class T, is_numeric<T>...>
inline T max(T _a, T _b) { return _a > _b ? _a : _b; }

// Minimum / Maximum of 3 /////
template<class T, is_numeric<T>...>
inline T min(T _a, T _b, T _c) { return min(min(_a, _b), _c); }
template<class T, is_numeric<T>...>
inline T max(T _a, T _b, T _c) { return max(max(_a, _b), _c); }

// Minimum / Maximum of 4 /////
template<class T, is_numeric<T>...>
inline T min(T _a, T _b, T _c, T _d) { return min(min(min(_a, _b), _c), _d); }
template<class T, is_numeric<T>...>
inline T max(T _a, T _b, T _c, T _d) { return max(max(max(_a, _b), _c), _d); }

// Clamp /////
template<class T, is_numeric<T>...>
inline T clamp(T _value, T _min, T _max) { return min(max(_value, _min), _max); }

// Linear Mix /////
template<class T, is_float<T>...>
inline T mix(T _a, T _b, T _factor) { return _a * (T(1.0) - _factor) + _b * _factor; }

// To Radians /////
template<class T, is_float<T>...>
inline T radians(T _deg) { return _deg * T(0.01745329251994329576923690768489); }

// To Degrees /////
template<class T, is_float<T>...>
inline T degrees(T _rad) { return _rad * T(57.29577951308232087679815481409246); }

}}
