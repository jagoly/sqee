// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <utility>

namespace sq {

//============================================================================//

/// Base for objects that can't be copied or moved.
struct NonCopyable
{
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};

//============================================================================//

/// Base for objects that can be moved but not copied.
struct MoveOnly
{
    MoveOnly() = default;

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

//============================================================================//

template <class...> struct Structure;

template <class Element> struct Structure<Element>
{
    Structure(const Element& element) : element{element} {}

    Element element;
};

template <class Element, class... Others> struct Structure<Element, Others...>
{
    Structure(const Element& element, const Others&... others) : element{element}, others{others...} {}

    Element element; Structure<Others...> others;
};

template<class... Elements> Structure(const Elements&...) -> Structure<Elements...>;

//============================================================================//

/// Holds two values of the same type.
template <class Type>
struct Swapper
{
    void swap() { std::swap(front, back); }

    Type front, back;
};

//============================================================================//

} // namespace sq
