// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <cstddef>
#include <type_traits>

//============================================================================//

// to use without sqee, replace this stuff with your own macros

#include <sqee/export.hpp>

#define WRENPLUS_API SQEE_API

#ifdef SQEE_DEBUG
  #define WRENPLUS_DEBUG
#endif

//============================================================================//

extern "C" {

typedef struct WrenVM WrenVM;
typedef struct WrenHandle WrenHandle;

} // extern "C"

//============================================================================//

namespace wren {

/// Specialise this for each foreign class you want to add to wren.
template <class Type> struct Traits : std::false_type {};

/// Check if Traits has been specialised for a given class type.
template <class Type> constexpr bool has_traits_v = Traits<Type>::value;

/// Specialise for getting and setting wren slots.
template <class Type, class = void> struct SlotHelper;

class WrenPlusVM;

} // namespace wren

//============================================================================//

#define WRENPLUS_TRAITS_HEADER(Type) \
template<> struct wren::Traits<Type> : std::true_type \
{ \
  static const char* const module; \
  static const char* const className; \
  static const size_t index; \
};

#define WRENPLUS_TRAITS_DEFINITION(Type, Module, ClassName) \
const char* const wren::Traits<Type>::module = Module; \
const char* const wren::Traits<Type>::className = ClassName; \
const size_t wren::Traits<Type>::index = wren::detail::generate_type_index();

//============================================================================//

#define WRENPLUS_ADD_METHOD(Pvm, Class, Method, Signature) \
  Pvm.register_method<&Class::Method>(Signature)

#define WRENPLUS_ADD_FIELD_R(Pvm, Class, Field, Name) \
  Pvm.register_method<wren::FieldGetter<&Class::Field>>(Name)

#define WRENPLUS_ADD_FIELD_W(Pvm, Class, Field, Name) \
  Pvm.register_method<wren::FieldSetter<&Class::Field>>(Name"=(_)")

#define WRENPLUS_ADD_FIELD_RW(Pvm, Class, Field, Name) \
  do { \
    WRENPLUS_ADD_FIELD_R(Pvm, Class, Field, Name); \
    WRENPLUS_ADD_FIELD_W(Pvm, Class, Field, Name); \
  } while (false)
