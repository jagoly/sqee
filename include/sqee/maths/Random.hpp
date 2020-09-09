// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/maths/Vectors.hpp>

#include <fmt/format.h>

#include <random>

namespace sq { namespace maths {

//============================================================================//

/// Callable object for generating random values within a range.
template <class T> struct RandomRange
{
    static_assert (
        std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t> ||
        std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, float> || std::is_same_v<T, double> ||
        detail::is_vector_v<T>, "unsupported type"
    );

    constexpr RandomRange() : data { T(0), T(0) } {}
    constexpr RandomRange(T min, T max) : min(min), max(max) {}

    template <class Generator>
    T operator()(Generator& gen) const
    {
        if (min == max)
        {
            if constexpr (std::is_arithmetic_v<T>)
                gen.discard(1u);

            if constexpr (detail::is_vector_v<T>)
                gen.discard(detail::vector_size_v<T>);

            return min;
        }

        if constexpr (std::is_integral_v<T>)
            return std::uniform_int_distribution(min, max)(gen);

        if constexpr (std::is_floating_point_v<T>)
            return std::uniform_real_distribution(min, max)(gen);

        if constexpr (detail::is_vector_v<T>)
        {
            T result(nullptr);
            for (int i = 0; i < detail::vector_size_v<T>; ++i)
                result[i] = RandomRange<detail::vector_type_t<T>>(min[i], max[i])(gen);
            return result;
        }
    }

    constexpr bool operator==(const RandomRange& other) const
    { return min == other.min && max == other.max; }

    constexpr bool operator!=(const RandomRange& other) const
    { return min != other.min || max != other.max; }

    union { T data[2]; struct { T min, max; }; };
};

//============================================================================//

} // namespace maths

template <class T>
constexpr const char* type_to_string(maths::RandomRange<T>)
{
    if constexpr (std::is_same_v<T, int16_t>) return "RandRangeInt16";
    if constexpr (std::is_same_v<T, uint16_t>) return "RandRangeUInt16";
    if constexpr (std::is_same_v<T, int32_t>) return "RandRangeInt32";
    if constexpr (std::is_same_v<T, uint32_t>) return "RandRangeUInt32";
    if constexpr (std::is_same_v<T, float>) return "RandRangeFloat";
    if constexpr (std::is_same_v<T, double>) return "RandRangeDouble";
    if constexpr (std::is_same_v<T, maths::Vector<2, int>>) return "RandRangeVec2I";
    if constexpr (std::is_same_v<T, maths::Vector<3, int>>) return "RandRangeVec3I";
    if constexpr (std::is_same_v<T, maths::Vector<4, int>>) return "RandRangeVec4I";
    if constexpr (std::is_same_v<T, maths::Vector<2, uint>>) return "RandRangeVec2U";
    if constexpr (std::is_same_v<T, maths::Vector<3, uint>>) return "RandRangeVec3U";
    if constexpr (std::is_same_v<T, maths::Vector<4, uint>>) return "RandRangeVec4U";
    if constexpr (std::is_same_v<T, maths::Vector<2, float>>) return "RandRangeVec2F";
    if constexpr (std::is_same_v<T, maths::Vector<3, float>>) return "RandRangeVec3F";
    if constexpr (std::is_same_v<T, maths::Vector<4, float>>) return "RandRangeVec4F";
    return "RandomRange<T>";
}

} // namespace sq

//============================================================================//

template <class T>
struct fmt::formatter<sq::maths::RandomRange<T>> : fmt::formatter<T>
{
    template <class FormatContext>
    auto format(const sq::maths::RandomRange<T>& range, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "{}(", sq::type_to_string(sq::maths::RandomRange<T>()));
        formatter<T>::format(range.min, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(range.max, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};

template <int S, class T>
struct fmt::formatter<sq::maths::RandomRange<sq::maths::Vector<S, T>>> : fmt::formatter<T>
{
    using VectorST = sq::maths::Vector<S, T>;

    template <class FormatContext>
    auto format(const sq::maths::RandomRange<VectorST>& range, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "{}((", sq::type_to_string(sq::maths::RandomRange<VectorST>()));
        formatter<T>::format(range.min.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(range.min.y, ctx);
        if constexpr (S >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (S >= 3) formatter<T>::format(range.min.z, ctx);
        if constexpr (S == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (S == 4) formatter<T>::format(range.min.w, ctx);
        fmt::format_to(ctx.out(), "), (");
        formatter<T>::format(range.max.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        formatter<T>::format(range.max.y, ctx);
        if constexpr (S >= 3) fmt::format_to(ctx.out(), ", ");
        if constexpr (S >= 3) formatter<T>::format(range.max.z, ctx);
        if constexpr (S == 4) fmt::format_to(ctx.out(), ", ");
        if constexpr (S == 4) formatter<T>::format(range.max.w, ctx);
        return fmt::format_to(ctx.out(), "))");
     }
};
