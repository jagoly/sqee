#pragma once

#include <concepts> // IWYU pragma: export
#include <type_traits> // IWYU pragma: export

namespace sq { //###############################################################

template <auto Value, auto... Values>
concept EqualToAny = ((Value == Values) || ...);

template <class Type, class... Types>
concept SameAsAny = (std::same_as<Type, Types> || ...);

//==============================================================================

/// Specialise with a StackString containing the canonical name of a type.
template <class Type>
constexpr auto type_name_v = nullptr;

template <class Type>
concept HasTypeName = !std::same_as<decltype(type_name_v<Type>), const std::nullptr_t>;

//==============================================================================

/// The underlying value of the first enum, either -1 or 0.
template <class Type>
constexpr auto enum_first_v = nullptr;

/// The number of enum values, excluding the first if it is -1.
template <class Type>
constexpr auto enum_count_v = nullptr;

/// An iterable containing all values for an enum.
template <class Type>
constexpr auto enum_values_v = nullptr;

/// An iterable containing all strings for an enum.
template <class Type>
constexpr auto enum_strings_v = nullptr;

template <class Type>
concept HasEnumHelper = !std::same_as<decltype(enum_first_v<Type>), const std::nullptr_t>;

} // namespace sq ##############################################################
