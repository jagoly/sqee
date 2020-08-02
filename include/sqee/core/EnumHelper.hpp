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

template <class T> using enum_base_t = typename enum_traits<T>::base;
template <class T> constexpr const auto enum_defined_v = enum_traits<T>::defined;
template <class T> constexpr const auto enum_first_v = enum_traits<T>::first;

} // namespace detail

//============================================================================//

/// An iterable containing all enumeration values.
template <class T> constexpr auto enum_items_v = detail::enum_traits<T>::items;

/// The value of the last enum plus one.
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

// todo: could be made to work differently with non-consecutive enums,
// rather than the current behaviour of just failing

#define SQEE_ENUM_ITEMS_INNER(Case) ,EnumType::Case

#define SQEE_ENUM_TO_STRING_INNER(Case) case EnumType::Case: return #Case;

#define SQEE_ENUM_FROM_STRING_INNER(Case) if (str == #Case) return EnumType::Case;

#define SQEE_ENUM_HELPER(Type, First, ...) \
template<> struct sq::detail::enum_traits<Type> \
{ \
    static constexpr bool defined = true; \
    using EnumType = Type; \
    using BaseType = std::underlying_type_t<Type>; \
    static constexpr auto first = BaseType(Type::First); \
    static_assert(first == 0 || first == -1, "unsupported first value"); \
    static constexpr auto items = std::array { Type::First SQEE_FOR_EACH(SQEE_ENUM_ITEMS_INNER, __VA_ARGS__) }; \
    static constexpr auto count = BaseType(items.size()) + first; \
    static const char* to_string(Type value) \
    { \
        switch (value) { SQEE_FOR_EACH(SQEE_ENUM_TO_STRING_INNER, First, __VA_ARGS__) } \
        throw std::invalid_argument(fmt::format("enum " #Type " to string: {}", value)); \
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
