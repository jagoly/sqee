#pragma once

#include <sqee/setup.hpp>
#include <sqee/helpers.hpp>

//============================================================================//

#ifndef SQEE_MSVC

#define DISABLE_WARNING_FLOAT_EQUALITY \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")

#define ENABLE_WARNING_FLOAT_EQUALITY \
_Pragma("GCC diagnostic pop")

#else

#define DISABLE_FLOAT_EQUALITY_WARNING
#define ENABLE_FLOAT_EQUALITY_WARNING

#endif

//============================================================================//

#define SQEE_MACRO_OVERLOAD(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
                            _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                            _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                            _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                            Name, ...) Name

//============================================================================//

#define SQEE_FOR_EACH_01(Functor, Arg) Functor(Arg)
#define SQEE_FOR_EACH_02(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_01(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_03(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_02(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_04(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_03(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_05(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_04(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_06(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_05(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_07(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_06(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_08(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_07(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_09(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_08(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_10(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_09(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_11(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_10(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_12(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_11(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_13(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_12(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_14(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_13(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_15(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_14(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_16(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_15(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_17(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_16(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_18(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_17(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_19(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_18(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_20(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_19(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_21(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_20(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_22(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_21(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_23(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_22(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_24(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_23(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_25(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_24(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_26(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_25(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_27(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_26(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_28(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_27(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_29(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_28(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_30(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_29(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_31(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_30(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_32(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_31(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_33(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_32(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_34(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_33(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_35(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_34(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_36(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_35(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_37(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_36(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_38(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_37(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_39(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_38(Functor, __VA_ARGS__)
#define SQEE_FOR_EACH_40(Functor, Arg, ...) Functor(Arg) SQEE_FOR_EACH_39(Functor, __VA_ARGS__)

#define SQEE_FOR_EACH(Functor, ...) \
    SQEE_MACRO_OVERLOAD ( __VA_ARGS__, \
        SQEE_FOR_EACH_40, SQEE_FOR_EACH_39, SQEE_FOR_EACH_38, SQEE_FOR_EACH_37, SQEE_FOR_EACH_36, \
        SQEE_FOR_EACH_35, SQEE_FOR_EACH_34, SQEE_FOR_EACH_33, SQEE_FOR_EACH_32, SQEE_FOR_EACH_31, \
        SQEE_FOR_EACH_30, SQEE_FOR_EACH_29, SQEE_FOR_EACH_28, SQEE_FOR_EACH_27, SQEE_FOR_EACH_26, \
        SQEE_FOR_EACH_25, SQEE_FOR_EACH_24, SQEE_FOR_EACH_23, SQEE_FOR_EACH_22, SQEE_FOR_EACH_21, \
        SQEE_FOR_EACH_20, SQEE_FOR_EACH_19, SQEE_FOR_EACH_18, SQEE_FOR_EACH_17, SQEE_FOR_EACH_16, \
        SQEE_FOR_EACH_15, SQEE_FOR_EACH_14, SQEE_FOR_EACH_13, SQEE_FOR_EACH_12, SQEE_FOR_EACH_11, \
        SQEE_FOR_EACH_10, SQEE_FOR_EACH_09, SQEE_FOR_EACH_08, SQEE_FOR_EACH_07, SQEE_FOR_EACH_06, \
        SQEE_FOR_EACH_05, SQEE_FOR_EACH_04, SQEE_FOR_EACH_03, SQEE_FOR_EACH_02, SQEE_FOR_EACH_01, \
    ) ( Functor, __VA_ARGS__ )

//============================================================================//

#define SWITCH(Value) { using SwitchValueT = std::decay_t<decltype(Value)>; switch (Value)

#define SWITCH_END }

#define SQEE_CASE_INNER(Case) case SwitchValueT::Case:

#define CASE(...) break; SQEE_FOR_EACH(SQEE_CASE_INNER, __VA_ARGS__)

#define CASE_DEFAULT break; default:

//============================================================================//

#define SQEE_ENUM_TO_STRING_INNER(Case) case EnumType::Case: return #Case;

#define SQEE_ENUM_FROM_STRING_INNER(Case) if (str == #Case) return EnumType::Case;

#define SQEE_ENUM_COUNT_INNER(Case) BaseType(EnumType::Case),

#define SQEE_ENUM_JSON_CONVERSION_DEFINITIONS_INNER(Case) \
    if (ref == #Case) { e = EnumType::Case; return; }

#define SQEE_ENUM_HELPER(Type, ...) \
template<> struct sq::EnumHelper<Type> \
{ \
    using EnumType = Type; \
    using BaseType = std::underlying_type_t<Type>; \
    static const char* to_string(Type value) \
    { \
        switch (value) { SQEE_FOR_EACH(SQEE_ENUM_TO_STRING_INNER, __VA_ARGS__) } \
        throw std::invalid_argument(build_string("enum ", #Type, " to string")); \
    } \
    static Type from_string(std::string_view str) \
    { \
        SQEE_FOR_EACH(SQEE_ENUM_FROM_STRING_INNER, __VA_ARGS__) \
        throw std::invalid_argument(build_string("enum ", #Type, " from string '", str, "'")); \
    } \
    static constexpr BaseType count = va_max(SQEE_FOR_EACH(SQEE_ENUM_COUNT_INNER, __VA_ARGS__) BaseType(0)) + 1; \
};
