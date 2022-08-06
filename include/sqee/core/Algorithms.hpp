// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <algorithm> // IWYU pragma: export
#include <map> // IWYU pragma: export
#include <vector> // IWYU pragma: export

namespace sq::algo {

//============================================================================//

template <class Container, class Value>
constexpr auto find(Container& container, const Value& value)
{
    return std::find(std::begin(container), std::end(container), value);
}

template <class Container, class Predicate>
constexpr auto find_if(Container& container, Predicate pred)
{
    return std::find_if(std::begin(container), std::end(container), pred);
}

template <class Container, class Value>
constexpr size_t count(Container& container, const Value& value)
{
    return std::count(std::begin(container), std::end(container), value);
}

template <class Container, class Predicate>
constexpr size_t count_if(Container& container, Predicate pred)
{
    return std::count_if(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool all_of(Container& container, Predicate pred)
{
    return std::all_of(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool any_of(Container& container, Predicate pred)
{
    return std::any_of(std::begin(container), std::end(container), pred);
}

template <class Container, class Predicate>
constexpr bool none_of(Container& container, Predicate pred)
{
    return std::none_of(std::begin(container), std::end(container), pred);
}

//============================================================================//

template <class Type, class Alloc, class Value>
inline size_t erase(std::vector<Type, Alloc>& container, const Value& value)
{
    const auto iter = std::remove(container.begin(), container.end(), value);
    const auto erased = std::distance(iter, container.end());
    container.erase(iter, container.end());
    return erased;
}

template <class Type, class Alloc, class Predicate>
inline size_t erase_if(std::vector<Type, Alloc>& container, Predicate pred)
{
    const auto iter = std::remove_if(container.begin(), container.end(), pred);
    const auto erased = std::distance(iter, container.end());
    container.erase(iter, container.end());
    return erased;
}

//============================================================================//

template <class Key, class Type, class Compare, class Alloc, class Predicate>
inline size_t erase_if(std::map<Key, Type, Compare, Alloc>& container, Predicate pred)
{
    const auto oldSize = container.size();
    for (auto iter = container.begin(), end = container.end(); iter != end;)
    {
        if (pred(*iter)) iter = container.erase(iter);
        else ++iter;
    }
    return oldSize - container.size();
}

//============================================================================//

} // namespace sq::algo
