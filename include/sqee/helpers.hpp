#pragma once

#include <memory>
#include <type_traits>

namespace sq {

//============================================================================//

template <class Type, class... Args>
std::unique_ptr<Type> make_unique_aggregate(Args&&... args)
{
    static_assert(std::is_aggregate_v<Type> == true);

    return std::unique_ptr<Type>(new Type{std::forward<Args>(args)...});
}

//============================================================================//

template <class...> struct Structure;

//----------------------------------------------------------------------------//

/// @cond

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

template <class...> struct index_in_pack;

//----------------------------------------------------------------------------//

/// @cond ignored

template <class T, class... Ts>
struct index_in_pack<T, T, Ts...> : std::integral_constant<size_t, 0> {};

template <class T, class A, class... Ts>
struct index_in_pack<T, A, Ts...> : std::integral_constant<size_t, 1 + index_in_pack<T, Ts...>::value> {};

template <class T, class... Ts>
constexpr auto index_in_pack_v = index_in_pack<T, Ts...>::value;

/// @endcond

//============================================================================//

} // namespace sq
