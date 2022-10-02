// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <utility>

namespace sq {

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
