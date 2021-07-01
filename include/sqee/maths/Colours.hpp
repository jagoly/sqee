// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Types.hpp>

namespace sq::maths {

//============================================================================//

/// Convert an HSV colour value to sRGB.
template <class T> constexpr Vector3<T> hsv_to_srgb(Vector3<T> hsv)
{
    static_assert(std::is_floating_point_v<T>);

    const int i = int(hsv[0] * T(6));
    const T f = hsv[0] * T(6) - T(i);
    const T p = hsv[2] * (T(1) - hsv[1]);
    const T q = hsv[2] * (T(1) - f * hsv[1]);
    const T t = hsv[2] * (T(1) - (T(1) - f) * hsv[1]);

    const int j = i % 6;

    if (j == 0) return { hsv.z, t, p };
    if (j == 1) return { q, hsv.z, p };
    if (j == 2) return { p, hsv.z, t };
    if (j == 3) return { p, q, hsv.z };
    if (j == 4) return { t, p, hsv.z };
    if (j == 5) return { hsv.z, p, q };
}

//============================================================================//

/// Convert an sRGB scalar to linear.
template <class T> constexpr T srgb_to_linear(T value)
{
    static_assert(std::is_floating_point_v<T>);

    if (value <= T(0.04045)) return value / T(12.92);
    return std::pow<T>(value + T(0.055) / T(1.055), T(2.4));
}

/// Convert an sRGB vector to linear.
template <class T> constexpr Vector3<T> srgb_to_linear(Vector3<T> rgb)
{
    static_assert(std::is_floating_point_v<T>);

    const T r = srgb_to_linear(rgb.r);
    const T g = srgb_to_linear(rgb.g);
    const T b = srgb_to_linear(rgb.b);

    return { r, g, b };
}

/// Convert an sRGB+A vector to linear.
template <class T> constexpr Vector4<T> srgb_to_linear(Vector4<T> rgba)
{
    static_assert(std::is_floating_point_v<T>);

    const T r = srgb_to_linear(rgba.r);
    const T g = srgb_to_linear(rgba.g);
    const T b = srgb_to_linear(rgba.b);

    return { r, g, b, rgba.a };
}

//============================================================================//

/// Compress a hdr colour into a 32 bit unsigned integer.
inline uint32_t hdr_to_e5bgr9(Vec3F rgb)
{
    constexpr float MAX_VALUE = float((1<<9)-1) / float(1<<9) * float(1<<5);

    const Vec3F clamped = maths::clamp(rgb, Vec3F(0.f), Vec3F(MAX_VALUE));
    const float maxComponent = maths::max(clamped.r, clamped.g, clamped.b);

    const float prelimExponent = std::max(-15.f - 1.f, std::floor(std::log2(maxComponent))) + 1.f + 15.f;
    const float maxExponent = std::floor(maxComponent / std::pow(2.f, (prelimExponent - 15.f - 9.f)) + 0.5f);
    const float exponent = maxExponent < std::pow(2.f, 9.f) ? prelimExponent : prelimExponent + 1.f;

    const Vec3U values = Vec3U(clamped / std::pow(2.f, (exponent - 15.f - 9.f)) + 0.5f);

    return uint(exponent) << 27 | values.b << 18 | values.g << 9 | values.r;
}

//============================================================================//

} // namespace sq::maths
