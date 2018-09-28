// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

namespace sq::algo {

//============================================================================//

/// Try to find a matching value within a range.
///
/// @param begin iterator to range start
/// @param end iterator to range end
/// @param value the value to search for
///
/// @result iterator to the element, or end

template <class Iter, class Value> constexpr
Iter find(Iter begin, Iter end, const Value& value)
{
    while (begin != end && !(*begin == value)) ++begin;
    return begin;
}

//----------------------------------------------------------------------------//

/// Try to find a non-matching value within a range.
///
/// @param begin iterator to range start
/// @param end iterator to range end
/// @param value the value to search for
///
/// @result iterator to the element, or end

template <class Iter, class Value> constexpr
Iter find_not(Iter begin, Iter end, const Value& value)
{
    while (begin != end && (*begin == value)) ++begin;
    return begin;
}

//----------------------------------------------------------------------------//

/// Try to find a value satisfying a predicate within a range.
///
/// @param begin iterator to range start
/// @param end iterator to range end
/// @param pred unary predicate callable
///
/// @result iterator to the element, or end

template <class Iter, class Predicate> constexpr
Iter find_if(Iter begin, Iter end, const Predicate& pred)
{
    while (begin != end && !pred(*begin)) ++begin;
    return begin;
}

//----------------------------------------------------------------------------//

/// Check if a matching value exists within a range.
///
/// @param begin iterator to range start
/// @param end iterator to range end
/// @param value the value to search for
///
/// @result true if found, otherwise false

template <class Iter, class Value> constexpr
bool exists(Iter begin, Iter end, const Value& value)
{
    return algo::find(begin, end, value) != end;
}

//----------------------------------------------------------------------------//

/// Check if a non-matching value exists within a range.
///
/// @param begin iterator to range start
/// @param end iterator to range end
/// @param value the value to search for
///
/// @result true if found, otherwise false

template <class Iter, class Value> constexpr
bool exists_not(Iter begin, Iter end, const Value& value)
{
    return algo::find_not(begin, end, value) != end;
}

//============================================================================//

/// Try to find a matching value in a container.
///
/// @param container an iterable container object
/// @param value the value to search for
///
/// @result iterator to the element, or end

template <class Container, class Value> constexpr
auto find(Container& container, const Value& value)
{
    return algo::find(container.begin(), container.end(), value);
}

//----------------------------------------------------------------------------//

/// Try to find a non-matching value in a container.
///
/// @param container an iterable container object
/// @param value the value to search for
///
/// @result iterator to the element, or end

template <class Container, class Value> constexpr
auto find_not(Container& container, const Value& value)
{
    return algo::find_not(container.begin(), container.end(), value);
}

//----------------------------------------------------------------------------//

/// Try to find a value satisfying a predicate in a container.
///
/// @param container an iterable container object
/// @param pred unary predicate callable
///
/// @result iterator to the element, or end

template <class Container, class Predicate> constexpr
auto find_if(Container& container, const Predicate& pred)
{
    return algo::find_if(container.begin(), container.end(), pred);
}

//----------------------------------------------------------------------------//

/// Check if a matching value exists in a container.
///
/// @param container an iterable container object
/// @param value the value to search for
///
/// @result true if found, otherwise false

template <class Container, class Value> constexpr
bool exists(Container& container, const Value& value)
{
    return algo::find(container, value) != container.end();
}

//----------------------------------------------------------------------------//

/// Check if a non-matching value exists in a container.
///
/// @param container an iterable container object
/// @param value the value to search for
///
/// @result true if found, otherwise false

template <class Container, class Value> constexpr
bool exists_not(Container& container, const Value& value)
{
    return algo::find_not(container, value) != container.end();
}

//============================================================================//

} // namespace sq::algo
