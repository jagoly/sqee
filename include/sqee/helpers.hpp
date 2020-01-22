#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

namespace sq {

//============================================================================//

inline const char* to_c_string(const std::string& arg) { return arg.c_str(); }
inline const char* to_c_string(const char* arg) { return arg; }

//============================================================================//

/// Template class for the SQEE_ENUM_HELPER macro.
template <class Type> struct EnumHelper;

/// Convert an enum to a string.
template <class Type>
const char* enum_to_string(Type value)
{
    return EnumHelper<Type>::to_string(value);
}

/// Convert a string to an enum.
template <class Type>
void enum_from_string(std::string_view str, Type& result)
{
    result = EnumHelper<Type>::from_string(str);
}

/// SFINAE overload for converting SQEE enums to c strings
template <class Type, class = typename EnumHelper<Type>::BaseType>
const char* to_c_string(Type arg)
{
    return EnumHelper<Type>::to_string(arg);
}

/// The value of the last enum plus one. Only useful for enums with consecutive values.
template <class Type>
constexpr const auto enum_count_v = EnumHelper<Type>::count();

/// An iterable containing all enumeration values.
template <class Type>
constexpr const auto enum_items_v = EnumHelper<Type>::items();

//----------------------------------------------------------------------------//

template <class Type, class... Args>
std::unique_ptr<Type> make_unique_aggregate(Args&&... args)
{
    static_assert(std::is_aggregate_v<Type> == true);
    return std::unique_ptr<Type>(new Type{std::forward<Args>(args)...});
}

//============================================================================//

template <class...> struct Structure;

//----------------------------------------------------------------------------//

/// @cond ignore

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

/// @endcond

//============================================================================//

template <class RealType>
struct alignas(RealType) UnitialisedType
{
    RealType* operator->() { return reinterpret_cast<RealType*>(_bytes); }
    std::byte _bytes[sizeof(RealType)];
};

//============================================================================//

template <class T> constexpr T va_min(T value) { return value; }
template <class T> constexpr T va_max(T value) { return value; }

template <class T, class... Ts> constexpr T va_min(T lhs, T rhs, Ts... others)
{
    static_assert(std::is_trivial_v<T>, "only use this for trivial types");
    return va_min(lhs < rhs ? lhs : rhs, others...);
}

template <class T, class... Ts> constexpr T va_max(T lhs, T rhs, Ts... others)
{
    static_assert(std::is_trivial_v<T>, "only use this for trivial types");
    return va_max(lhs > rhs ? lhs : rhs, others...);
}

//============================================================================//

template <class...> struct index_in_pack;

//----------------------------------------------------------------------------//

/// @cond ignore

template <class T, class... Ts>
struct index_in_pack<T, T, Ts...> : std::integral_constant<size_t, 0> {};

template <class T, class A, class... Ts>
struct index_in_pack<T, A, Ts...> : std::integral_constant<size_t, 1 + index_in_pack<T, Ts...>::value> {};

template <class T, class... Ts>
constexpr auto index_in_pack_v = index_in_pack<T, Ts...>::value;

/// @endcond

//============================================================================//

/// @cond ignore

namespace detail {

template <class> struct StringSize;

template <size_t N> struct StringSize <const char[N]>
{ static constexpr size_t size(const char(&)[N]) { return N - 1u; } };

template <> struct StringSize <std::string_view>
{ static constexpr size_t size(const std::string_view& s) { return s.size(); } };

template <> struct StringSize <char>
{ static constexpr size_t size(char) { return 1; } };

template <> struct StringSize <std::string>
{ static size_t size(const std::string& s) { return s.size(); } };

template <> struct StringSize <const char*>
{ static size_t size(const char* s) { return std::strlen(s); } };

template <class T> inline size_t string_size(T&& str)
{
    using value_t = std::remove_reference_t<T>;
    if constexpr (std::is_array_v<value_t>) return StringSize<value_t>::size(str);
    else return StringSize<std::remove_cv_t<value_t>>::size(str);
}

} // namespace detail

/// @endcond

//----------------------------------------------------------------------------//

template <class... Args>
inline std::string build_string(Args&&... args)
{
    std::string result;
    result.reserve((detail::string_size(std::forward<Args>(args)) + ...));
    ((result += args), ...);
    return result;
}

//============================================================================//

} // namespace sq

//============================================================================//

/// Write an enum as a string to a stream.
template <class Type, size_t = sizeof(sq::EnumHelper<Type>)>
std::ostream& operator<<(std::ostream& os, const Type& arg)
{
    return os << sq::EnumHelper<Type>::to_string(arg);
}
