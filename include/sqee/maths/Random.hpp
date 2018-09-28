#pragma once

#include <random>

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq::maths {

//============================================================================//

template <class T> struct RandomScalar
{
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

    using DistType = std::conditional_t < std::is_integral_v<T>,
                     std::uniform_int_distribution<T>, std::uniform_real_distribution<T> >;

    RandomScalar(T min, T max) : min(min), max(max)
    { SQASSERT(min <= max, ""); }

    template <class Generator>
    T operator()(Generator& gen) { return DistType(min, max)(gen); }

    const T min, max;
};

template<class T> RandomScalar(T, T) -> RandomScalar<T>;

//============================================================================//

template <int S, class T> struct RandomVector
{
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

    using DistType = std::conditional_t < std::is_integral_v<T>,
                     std::uniform_int_distribution<T>, std::uniform_real_distribution<T> >;

    RandomVector(Vector<S, T> min, Vector<S, T> max) : min(min), max(max)
    { SQASSERT(maths::all_less_equal(min, max), ""); }

    template <class Generator>
    Vector<S, T> operator()(Generator& gen)
    {
        Vector<S, T> result;
        for (int i = 0; i < S; ++i)
            result[i] = DistType(min[i], max[i])(gen);
        return result;
    }

    const Vector<S, T> min, max;
};

template<int S, class T> RandomVector(Vector<S, T>, Vector<S, T>) -> RandomVector<S, T>;

//============================================================================//

} // namespace sq::maths
