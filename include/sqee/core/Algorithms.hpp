// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <algorithm> // IWYU pragma: export
#include <functional>

namespace sq::algo {

//============================================================================//

template <class Value>
constexpr auto pred_equal_to(const Value& value)
{
    return [&value](const Value& item) { return item == value; };
}

template <class Value>
constexpr auto pred_not_equal_to(const Value& value)
{
    return [&value](const Value& item) { return item != value; };
}

template <class Value>
constexpr auto pred_greater(const Value& value)
{
    return [&value](const Value& item) { return item > value; };
}

template <class Value>
constexpr auto pred_less(const Value& value)
{
    return [&value](const Value& item) { return item < value; };
}

template <class Value>
constexpr auto pred_greater_equal(const Value& value)
{
    return [&value](const Value& item) { return item >= value; };
}

template <class Value>
constexpr auto pred_less_equal(const Value& value)
{
    return [&value](const Value& item) { return item <= value; };
}

//============================================================================//

template <class Container, class Value>
constexpr auto find(Container& container, const Value& value)
{
    return std::find(std::begin(container), std::end(container), value);
}

template <class Container, class Predicate>
constexpr auto find_if(Container& container, const Predicate& pred)
{
    return std::find_if(std::begin(container), std::end(container), pred);
}

template <class Container, class Value>
constexpr auto count(Container& container, const Value& value)
{
    return std::count(std::begin(container), std::end(container), value);
}

template <class Container, class Predicate>
constexpr auto count_if(Container& container, const Predicate& pred)
{
    return std::count_if(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool all_of(Container& container, const Predicate& pred)
{
    return std::all_of(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool any_of(Container& container, const Predicate& pred)
{
    return std::any_of(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool none_of(Container& container, const Predicate& pred)
{
    return std::none_of(std::begin(container), std::end(container), pred);
}

//============================================================================//

} // namespace sq::algo
