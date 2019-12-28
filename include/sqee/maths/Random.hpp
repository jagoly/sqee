#pragma once

#include <random>

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>
#include <sqee/misc/StaticVector.hpp>

namespace sq::maths {

//============================================================================//

/// Vector version of std::uniform_int_distribution / std::uniform_real_distribution
template <class VecST>
struct UniformVectorDistribution
{
    constexpr static const int Size = traits::VecSize<VecST>;
    using Type = traits::VecType<VecST>;

    using DistType =
        std::conditional_t<std::is_integral_v<Type>, std::uniform_int_distribution<Type>,
        std::conditional_t<std::is_floating_point_v<Type>, std::uniform_real_distribution<Type>,
    void>>;

    UniformVectorDistribution()
    {
        for (int i = 0; i < Size; ++i)
            distros[i] = DistType(Type(0), Type(1));
    }

    UniformVectorDistribution(const VecST& min, const VecST& max)
    {
        for (int i = 0; i < Size; ++i)
            distros[i] = DistType(min[i], max[i]);
    }

    template <class Generator>
    VecST operator()(Generator& gen)
    {
        VecST result(nullptr);
        for (int i = 0; i < Size; ++i)
            result[i] = distros[i](gen);
        return result;
    }

    DistType distros[Size];
};

//============================================================================//

/// Callable object for generating random values within a range.
template <class T>
struct RandomRange
{
    using DistType =
        std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>,
        std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
        UniformVectorDistribution<T>
    >>;

    RandomRange() : data { T(0), T(0) } {}

    RandomRange(T min, T max) : min(min), max(max) {}

    template <class Generator> T operator()(Generator& gen)
    {
        return DistType(min, max)(gen);
    }

    constexpr bool operator==(const RandomRange& other) const
    {
        return min == other.min && max == other.max;
    }

    constexpr bool operator!=(const RandomRange& other) const
    {
        return min != other.min || max != other.max;
    }

    union { T data[2]; struct { T min, max; }; };
};

template<class T> RandomRange(T, T) -> RandomRange<T>;

//============================================================================//

} // namespace sq::maths
