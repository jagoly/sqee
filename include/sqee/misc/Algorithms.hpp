#pragma once

#include <sqee/setup.hpp>

namespace sq { namespace algo {

//============================================================================//

template <class Iter, class Value> constexpr
Iter find(Iter begin, Iter end, const Value& value)
{
    while (begin != end && *begin != value) ++begin;
    return begin;
}

template <class Iter, class Predicate> constexpr
Iter find_if(Iter begin, Iter end, const Predicate& pred)
{
    while (begin != end && !pred(*begin)) ++begin;
    return begin;
}

template <class Iter, class Value> constexpr
bool exists(Iter begin, Iter end, const Value& value)
{
    return algo::find(begin, end, value) != end;
}

//============================================================================//

template <class Container, class Value> constexpr
auto find(Container& container, const Value& value)
{
    return algo::find(container.begin(), container.end(), value);
}

template <class Container, class Predicate> constexpr
auto find_if(Container& container, const Predicate& pred)
{
    return algo::find_if(container.begin(), container.end(), pred);
}

template <class Container, class Value> constexpr
bool exists(Container& container, const Value& value)
{
    return algo::find(container, value) != container.end();
}

//============================================================================//

}} // namespace sq::algo
