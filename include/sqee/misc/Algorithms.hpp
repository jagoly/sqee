#pragma once

#include <sqee/setup.hpp>

namespace sq {
namespace algo {

template<class Iter, class Value> inline
Iter find(Iter begin, Iter end, const Value& value) {
    while (begin != end && *begin != value) ++begin; return begin;
}

template<class Iter, class Value> inline
Iter find_first(Iter begin, Iter end, const Value& value) {
    while (begin != end && begin->first != value) ++begin; return begin;
}

template<class Iter, class Predicate> inline
Iter find_if(Iter begin, Iter end, const Predicate& pred) {
    while (begin != end && !pred(*begin)) ++begin; return begin;
}

template<class Container, class Value> inline
auto find(Container& container, const Value& value) {
    return sq::algo::find(container.begin(), container.end(), value);
}

template<class Container, class Value> inline
auto find_first(Container& container, const Value& value) {
    return sq::algo::find_first(container.begin(), container.end(), value);
}

template<class Container, class Predicate> inline
auto find_if(Container& container, const Predicate& pred) {
    return sq::algo::find_if(container.begin(), container.end(), pred);
}

template<class VectorA, class VectorB, class Predicate> inline
void multi_erase_if(VectorA& vecA, VectorB& vecB, const Predicate& pred)
{
    SQASSERT(vecA.size() == vecB.size(), "sizes do not match");

    const auto iter = sq::algo::find_if(vecA, pred);
    auto first = size_t(iter - vecA.begin());

    const size_t vectorSize = vecA.size();
    if (first == vectorSize) return;

    for (size_t i = first; ++i != vectorSize;)
    {
        if (pred(vecA[i])) continue;

        vecA[first] = std::move(vecA[i]);
        vecB[first] = std::move(vecB[i]);

        first += 1u;
    }

    vecA.erase(vecA.begin() + first, vecA.end());
    vecB.erase(vecB.begin() + first, vecB.end());
}

}}
