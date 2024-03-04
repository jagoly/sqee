// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/core/Macros.hpp>
#include <sqee/core/TypeTraits.hpp>
#include <sqee/misc/StackString.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace sq {

//============================================================================//

/// Try to convert an enum to a string.
template <HasEnumHelper Type>
constexpr std::optional<const char*> enum_to_string_safe(Type arg) noexcept
{
    for (size_t i = 0u; i < enum_values_v<Type>.size(); ++i)
        if (arg == enum_values_v<Type>[i])
            return enum_strings_v<Type>[i];

    return std::nullopt;
}

/// Convert an enum to a string.
template <HasEnumHelper Type>
constexpr const char* enum_to_string(Type arg)
{
    if (auto opt = enum_to_string_safe<Type>(arg)) return *opt;

    throw std::runtime_error(fmt::format("enum_to_string<{}>({})", type_name_v<Type>.c_str(), std::underlying_type_t<Type>(arg)));
}

/// Try to convert a string to an enum.
template <HasEnumHelper Type>
constexpr std::optional<Type> enum_from_string_safe(std::string_view arg) noexcept
{
    for (size_t i = 0u; i < enum_strings_v<Type>.size(); ++i)
        if (arg == enum_strings_v<Type>[i])
            return enum_values_v<Type>[i];

    return std::nullopt;
}

/// Convert a string to an enum.
template <HasEnumHelper Type>
constexpr Type enum_from_string(std::string_view arg)
{
    if (auto opt = enum_from_string_safe<Type>(arg)) return *opt;

    throw std::runtime_error(fmt::format("enum_from_string<{}>({})", type_name_v<Type>.c_str(), arg));
}

//============================================================================//

} // namespace sq

//============================================================================//

#define SQEE_ENUM_HELPER(Type, First, ...) \
template <> inline constexpr auto sq::type_name_v<Type> = sq::StackString(#Type); \
template <> inline constexpr auto sq::enum_first_v<Type> = std::underlying_type_t<Type>(Type::First); \
template <> inline constexpr auto sq::enum_count_v<Type> = std::underlying_type_t<Type>(int(Type::First) + SQEE_COUNT_ARGS(First, __VA_ARGS__)); \
template <> inline constexpr auto sq::enum_values_v<Type> = []() { using enum Type; return std::array { First, __VA_ARGS__ }; }(); \
template <> inline constexpr auto sq::enum_strings_v<Type> = []() { return std::array { SQEE_STRINGIFY_ARGS(First, __VA_ARGS__) }; }();

//============================================================================//

template <sq::HasEnumHelper Type>
struct fmt::formatter<Type> : fmt::formatter<const char*>
{
    template <class FormatContext>
    constexpr auto format(const Type& value, FormatContext& ctx) const
    {
        return fmt::formatter<const char*>::format(sq::enum_to_string(value), ctx);
    }
};
