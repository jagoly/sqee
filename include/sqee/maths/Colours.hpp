// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Scalar.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq::maths { //########################################################

/// Convert an HSV colour value to sRGB.
inline Vec3F hsv_to_srgb(Vec3F hsv)
{
    const int i = int(hsv.x * 6.f);

    const float f = hsv.x * 6.f - float(i);
    const float p = hsv.z * (1.f - hsv.y);
    const float q = hsv.z * (1.f - f * hsv.y);
    const float t = hsv.z * (1.f - (1.f - f) * hsv.y);

    const int j = i % 6;

    if (j == 0) return { hsv.z, t, p };
    if (j == 1) return { q, hsv.z, p };
    if (j == 2) return { p, hsv.z, t };
    if (j == 3) return { p, q, hsv.z };
    if (j == 4) return { t, p, hsv.z };
    if (j == 5) return { hsv.z, p, q };

    return {}; // unreachable
}

//==============================================================================

/// Convert an sRGB scalar to linear.
inline float srgb_to_linear(float value)
{
    if (value <= 0.04045f) return value / 12.92f;
    return std::pow((value + 0.055f) / 1.055f, 2.4f);
}

/// Convert an sRGB vector to linear.
inline Vec3F srgb_to_linear(Vec3F rgb)
{
    const float r = srgb_to_linear(rgb.x);
    const float g = srgb_to_linear(rgb.y);
    const float b = srgb_to_linear(rgb.z);

    return { r, g, b };
}

/// Convert an sRGB+A vector to linear.
inline Vec4F srgb_to_linear(Vec4F rgba)
{
    const float r = srgb_to_linear(rgba.x);
    const float g = srgb_to_linear(rgba.y);
    const float b = srgb_to_linear(rgba.z);

    return { r, g, b, rgba.w };
}

//==============================================================================

/// Compress a hdr colour into a 32 bit unsigned integer.
inline uint32_t hdr_to_e5bgr9(Vec3F rgb)
{
    constexpr float MAX_VALUE = float((1<<9)-1) / float(1<<9) * float(1<<5);

    const Vec3F clamped = maths::clamp(rgb, Vec3F(0.f), Vec3F(MAX_VALUE));
    const float maxComponent = maths::max(clamped.x, clamped.y, clamped.z);

    const float prelimExponent = std::max(-15.f - 1.f, std::floor(std::log2(maxComponent))) + 1.f + 15.f;
    const float maxExponent = std::floor(maxComponent / std::pow(2.f, (prelimExponent - 15.f - 9.f)) + 0.5f);
    const float exponent = maxExponent < std::pow(2.f, 9.f) ? prelimExponent : prelimExponent + 1.f;

    const Vec3U values = Vec3U(clamped / std::pow(2.f, (exponent - 15.f - 9.f)) + 0.5f);

    return uint(exponent) << 27 | values.z << 18 | values.y << 9 | values.x;
}

} // namespace sq::maths #######################################################
