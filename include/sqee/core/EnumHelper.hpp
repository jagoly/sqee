// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Macros.hpp>

#include <fmt/format.h>

#include <array>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <cstdint> // IWYU pragma: export

namespace sq {

//============================================================================//

namespace detail {

template <class T> struct enum_traits
{
    static constexpr bool defined = false;
};

template <class T> using enum_base_t = typename detail::enum_traits<T>::base_type;
template <class T> constexpr auto enum_defined_v = enum_traits<T>::defined;

} // namespace detail

//============================================================================//

/// An iterable containing all enumeration values.
template <class T> constexpr auto enum_items_v = detail::enum_traits<T>::items;

/// The underlying value of the first enum.
template <class T> constexpr auto enum_first_v = detail::enum_traits<T>::first;

/// The number of enum values, excluding the first if it is -1.
template <class T> constexpr auto enum_count_v = detail::enum_traits<T>::count;

//============================================================================//

/// Convert an enum to a string.
template <class Type>
const char* enum_to_string(Type value)
{
    return detail::enum_traits<Type>::to_string(value);
}

/// Convert a string to an enum.
template <class Type>
void enum_from_string(std::string_view str, Type& result)
{
    result = detail::enum_traits<Type>::from_string(str);
}

/// SFINAE overload for converting enums to c strings.
template <class Type>
std::enable_if_t<detail::enum_defined_v<Type>, const char*> to_c_string(Type arg)
{
    return enum_to_string(arg);
}

//============================================================================//

} // namespace sq

//============================================================================//

#define SQEE_ENUM_ITEMS_INNER(Case) ,enum_type::Case

#define SQEE_ENUM_TO_STRING_INNER(Case) case enum_type::Case: return #Case;

#define SQEE_ENUM_FROM_STRING_INNER(Case) if (str == #Case) return enum_type::Case;

#define SQEE_ENUM_HELPER(Type, First, ...) \
template<> struct sq::detail::enum_traits<Type> \
{ \
    static constexpr bool defined = true; \
    using enum_type = Type; \
    using base_type = std::underlying_type_t<Type>; \
    static constexpr auto items = std::array { Type::First SQEE_FOR_EACH(SQEE_ENUM_ITEMS_INNER, __VA_ARGS__) }; \
    static constexpr auto first = base_type(items.front()); \
    static_assert(first == 0 || first == -1, "unsupported first value"); \
    static constexpr auto count = base_type(items.size()) + first; \
    static const char* to_string(Type value) \
    { \
        switch (value) { SQEE_FOR_EACH(SQEE_ENUM_TO_STRING_INNER, First, __VA_ARGS__) } \
        throw std::invalid_argument(fmt::format("enum " #Type " to string: {}", base_type(value))); \
    } \
    static Type from_string(std::string_view str) \
    { \
        SQEE_FOR_EACH(SQEE_ENUM_FROM_STRING_INNER, First, __VA_ARGS__) \
        throw std::invalid_argument(fmt::format("enum " #Type " from string: '{}'", str)); \
    } \
};

//============================================================================//

template <class Type>
struct fmt::formatter<Type, char, std::enable_if_t<sq::detail::enum_defined_v<Type>>> : fmt::formatter<const char*>
{
    template <class FormatContext>
    auto format(const Type& value, FormatContext& ctx)
    {
        return formatter<const char*>::format(sq::enum_to_string(value), ctx);
    }
};
