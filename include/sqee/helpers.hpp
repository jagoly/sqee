#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

namespace sq {

//============================================================================//

namespace literals {

constexpr int8_t operator ""_i8(unsigned long long v)
{ return static_cast<int8_t>(v); }

constexpr uint8_t operator ""_u8(unsigned long long v)
{ return static_cast<uint8_t>(v); }

constexpr int16_t operator ""_i16(unsigned long long v)
{ return static_cast<int16_t>(v); }

constexpr uint16_t operator ""_u16(unsigned long long v)
{ return static_cast<uint16_t>(v); }

constexpr int32_t operator ""_i32(unsigned long long v)
{ return static_cast<int32_t>(v); }

constexpr uint32_t operator ""_u32(unsigned long long v)
{ return static_cast<uint32_t>(v); }

constexpr int64_t operator ""_i64(unsigned long long v)
{ return static_cast<int64_t>(v); }

constexpr uint64_t operator ""_u64(unsigned long long v)
{ return static_cast<uint64_t>(v); }

} // namespace literals

//============================================================================//

template <class EnumType>
std::underlying_type_t<EnumType>& underlying_reference(EnumType& ref)
{
    return reinterpret_cast<std::underlying_type_t<EnumType>&>(ref);
}

//----------------------------------------------------------------------------//

template <class Type, class... Args>
std::unique_ptr<Type> make_unique_aggregate(Args&&... args)
{
    // todo: not yet supported with clang + libstdc++
    #ifndef SQEE_CLANG
    static_assert(std::is_aggregate_v<Type> == true);
    #endif

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

// MSVC doesn't support class template deduction yet
#ifndef SQEE_MSVC
template<class... Elements> Structure(const Elements&...) -> Structure<Elements...>;
#endif

/// @endcond

//============================================================================//

template <class RealType>
struct alignas(RealType) UnitialisedType
{
    RealType* operator->() { return reinterpret_cast<RealType*>(_bytes); }
    std::byte _bytes[sizeof(RealType)];
};

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
  (result.append(args), ...);
  return result;
}

//============================================================================//

} // namespace sq
