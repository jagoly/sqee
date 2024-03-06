#pragma once

#include <sqee/maths/Vectors.hpp>

#include <random> // IWYU pragma: export

namespace sq::maths { //########################################################

using RandNumGen = std::mt19937;

/// Callable object for generating random values within a range.
template <class T> struct RandomRange
{
    constexpr RandomRange() : min(T(0)), max(T(0)) {}
    constexpr RandomRange(T min, T max) : min(min), max(max) {}

    T operator()(RandNumGen& gen) const requires std::integral<T>
    {
        if (min == max) { gen.discard(1); return min; }
        return std::uniform_int_distribution(min, max)(gen);
    };

    T operator()(RandNumGen& gen) const requires std::floating_point<T>
    {
        if (min == max) { gen.discard(1); return min; }
        return std::uniform_real_distribution(min, max)(gen);
    };

    T operator()(RandNumGen& gen) const requires AnyVector<T>
    {
        return [&]<int VecS, class VecT>(Vector<VecS, VecT> result)
        {
            for (int i = 0; i < VecS; ++i)
                result[i] = RandomRange<VecT>(min[i], max[i])(gen);
            return result;
        }
        (T(nullptr));
    };

    constexpr bool operator==(const RandomRange& other) const
    {
        return min == other.min && max == other.max;
    }

    T min, max;
};

} // namespace sq::maths #######################################################

#include <sqee/core/TypeNames.hpp>

#include <fmt/format.h>

template <class T>
struct fmt::formatter<sq::maths::RandomRange<T>>
{
    fmt::formatter<T> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    FormatContext::iterator format(const sq::maths::RandomRange<T>& rr, FormatContext& ctx) const
    {
        ctx.advance_to(fmt::detail::write(ctx.out(), sq::type_name_v<sq::maths::RandomRange<T>>.c_str()));
        ctx.advance_to(fmt::detail::write(ctx.out(), '('));
        ctx.advance_to(base.format(rr.min, ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(rr.max, ctx));
        return fmt::detail::write(ctx.out(), ')');
    }
};

template <int S, class T>
struct fmt::formatter<sq::maths::RandomRange<sq::maths::Vector<S, T>>>
{
    fmt::formatter<T> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    FormatContext::iterator format(const sq::maths::RandomRange<sq::maths::Vector<S, T>>& rr, FormatContext& ctx) const
    {
        ctx.advance_to(fmt::detail::write(ctx.out(), sq::type_name_v<sq::maths::RandomRange<sq::maths::Vector<S, T>>>.c_str()));
        ctx.advance_to(fmt::detail::write(ctx.out(), "(("));
        ctx.advance_to(base.format(rr.min[0], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(rr.min[1], ctx));
        if constexpr (S >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S >= 3) ctx.advance_to(base.format(rr.min[2], ctx));
        if constexpr (S == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S == 4) ctx.advance_to(base.format(rr.min[3], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), "), ("));
        ctx.advance_to(base.format(rr.max[0], ctx));
        ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        ctx.advance_to(base.format(rr.max[1], ctx));
        if constexpr (S >= 3) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S >= 3) ctx.advance_to(base.format(rr.max[2], ctx));
        if constexpr (S == 4) ctx.advance_to(fmt::detail::write(ctx.out(), ", "));
        if constexpr (S == 4) ctx.advance_to(base.format(rr.max[3], ctx));
        return fmt::detail::write(ctx.out(), "))");
    }
};
