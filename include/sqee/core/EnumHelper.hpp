// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Macros.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace sq {

//============================================================================//

/// Specialise using the SQEE_ENUM_HELPER macro.
template <class T> struct EnumTraits : std::false_type {};

/// Check if EnumTraits has been specialised for a given enum type.
template <class T> constexpr bool has_enum_traits_v = EnumTraits<T>::value;

/// An iterable containing all enumeration values.
template <class T> constexpr auto enum_items_v = EnumTraits<T>::items;

/// The underlying value of the first enum.
template <class T> constexpr auto enum_first_v = EnumTraits<T>::first;

/// The number of enum values, excluding the first if it is -1.
template <class T> constexpr auto enum_count_v = EnumTraits<T>::count;

//============================================================================//

/// Convert an enum to a string.
template <class Type>
constexpr const char* enum_to_string(Type value)
{
    return EnumTraits<Type>::to_string(value);
}

/// Try to convert an enum to a string.
template <class Type>
constexpr std::optional<const char*> enum_to_string_safe(Type value)
{
    return EnumTraits<Type>::to_string_safe(value);
}

/// Convert a string to an enum.
template <class Type>
constexpr Type enum_from_string(std::string_view sv)
{
    return EnumTraits<Type>::from_string(sv);
}

/// Try to convert a string to an enum.
template <class Type>
constexpr std::optional<Type> enum_from_string_safe(std::string_view sv)
{
    return EnumTraits<Type>::from_string_safe(sv);
}

/// SFINAE overload for converting enums to c strings.
template <class Type>
std::enable_if_t<has_enum_traits_v<Type>, const char*> to_c_string(Type arg)
{
    return enum_to_string(arg);
}

//============================================================================//

} // namespace sq

//============================================================================//

#define SQEE_ENUM_ITEMS_INNER(Case) ,enum_type::Case

#define SQEE_ENUM_TO_STRING_INNER(Case) case enum_type::Case: return #Case;

#define SQEE_ENUM_FROM_STRING_INNER(Case) if (sv == #Case) return enum_type::Case;

#define SQEE_ENUM_HELPER(Type, First, ...) \
template<> struct sq::EnumTraits<Type> : std::true_type \
{ \
    using enum_type = Type; \
    using base_type = std::underlying_type_t<Type>; \
    static constexpr auto items = std::array { Type::First SQEE_FOR_EACH(SQEE_ENUM_ITEMS_INNER, __VA_ARGS__) }; \
    static constexpr auto first = base_type(items.front()); \
    static_assert(first == 0 || first == -1, "unsupported first value"); \
    static constexpr auto count = base_type(items.size()) + first; \
    static constexpr const char* to_string(Type value) \
    { \
        switch (value) { SQEE_FOR_EACH(SQEE_ENUM_TO_STRING_INNER, First, __VA_ARGS__) } \
        throw std::runtime_error(fmt::format("enum " #Type " to string: {}", base_type(value))); \
    } \
    static constexpr Type from_string(std::string_view sv) \
    { \
        SQEE_FOR_EACH(SQEE_ENUM_FROM_STRING_INNER, First, __VA_ARGS__) \
        throw std::runtime_error(fmt::format("enum " #Type " from string: '{}'", sv)); \
    } \
    static constexpr std::optional<const char*> to_string_safe(Type value) \
    { \
        switch (value) { SQEE_FOR_EACH(SQEE_ENUM_TO_STRING_INNER, First, __VA_ARGS__) } \
        return std::nullopt; \
    } \
    static constexpr std::optional<Type> from_string_safe(std::string_view sv) \
    { \
        SQEE_FOR_EACH(SQEE_ENUM_FROM_STRING_INNER, First, __VA_ARGS__) \
        return std::nullopt; \
    } \
};

//============================================================================//

template <class Type>
struct fmt::formatter<Type, char, std::enable_if_t<sq::has_enum_traits_v<Type>>> : fmt::formatter<const char*>
{
    template <class FormatContext>
    auto format(const Type& value, FormatContext& ctx)
    {
        return formatter<const char*>::format(sq::enum_to_string(value), ctx);
    }
};
