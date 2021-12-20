// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Types.hpp>

#include <fmt/core.h>

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

namespace sq {

//============================================================================//

inline const char* to_c_string(const char* arg)
{
    return arg;
}

inline const char* to_c_string(const String& arg)
{
    return arg.c_str();
}

template <size_t Capacity>
inline const char* to_c_string(const StackString<Capacity>& arg)
{
    return arg.c_str();
}

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

template <size_t Size>
constexpr size_t string_length(const char(&)[Size])
{
    return Size - 1u;
}

template <size_t Capacity>
constexpr size_t string_length(const StackString<Capacity>& ss)
{
    return ss.length();
}

constexpr size_t string_length(char)
{
    return 1u;
}

constexpr size_t string_length(const std::string_view& sv)
{
    return sv.length();
}

template <class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>>
inline size_t string_length(const CharT* const& cstr)
{
    return std::strlen(cstr);
}

//----------------------------------------------------------------------------//

/// Function to join a bunch of strings with a single allocation.
template <class... Args>
inline String build_string(Args&&... args)
{
    String result;
    result.reserve((string_length(std::forward<Args>(args)) + ...));
    ((result += args), ...);
    return result;
}

//============================================================================//

/// Slightly more ergonomic wrapper around fmt::format_to.
template <class... Args>
inline String& format_append(String& output, StringView str, const Args&... args)
{
    fmt::format_to(std::back_inserter(output), str, args...);
    return output;
}

//============================================================================//

} // namespace sq
