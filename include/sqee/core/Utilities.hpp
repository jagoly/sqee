// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Types.hpp>

#include <memory>
#include <type_traits>

#include <cstddef>
#include <cstring>

namespace sq {

//============================================================================//

inline const char* to_c_string(const char* arg) { return arg; }
inline const char* to_c_string(const String& arg) { return arg.c_str(); }
template <size_t C> inline const char* to_c_string(const StackString<C>& arg) { return arg.c_str(); }

//============================================================================//

template <class Type, class... Args>
std::unique_ptr<Type> make_unique_aggregate(Args&&... args)
{
    static_assert(std::is_aggregate_v<Type> == true);
    return std::unique_ptr<Type>(new Type{std::forward<Args>(args)...});
}

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

namespace detail {

template <class> struct StringSize;

template <size_t N> struct StringSize <const char[N]>
{ static constexpr size_t size(const char(&)[N]) { return N - 1u; } };

template <> struct StringSize <StringView>
{ static constexpr size_t size(const StringView& s) { return s.size(); } };

template <> struct StringSize <char>
{ static constexpr size_t size(char) { return 1; } };

template <> struct StringSize <String>
{ static size_t size(const String& s) { return s.size(); } };

template <> struct StringSize <const char*>
{ static size_t size(const char* s) { return std::strlen(s); } };

template <class T> inline size_t string_size(T&& str)
{
    using value_t = std::remove_reference_t<T>;
    if constexpr (std::is_array_v<value_t>) return StringSize<value_t>::size(str);
    else return StringSize<std::remove_cv_t<value_t>>::size(str);
}

} // namespace detail

//----------------------------------------------------------------------------//

/// Function to join a bunch of strings with a single allocation.
template <class... Args>
inline String build_string(Args&&... args)
{
    String result;
    result.reserve((detail::string_size(std::forward<Args>(args)) + ...));
    ((result += args), ...);
    return result;
}

//============================================================================//

} // namespace sq
