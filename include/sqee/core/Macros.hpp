// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

//============================================================================//

#ifndef SQEE_MSVC

// sometimes, I really do want to know if floats are equal

#define DISABLE_WARNING_FLOAT_EQUALITY() \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")

#define ENABLE_WARNING_FLOAT_EQUALITY() \
_Pragma("GCC diagnostic pop")

// imgui colour macros use old style casts, I like this warning otherwise

#define DISABLE_WARNING_OLD_STYLE_CAST() \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")

#define ENABLE_WARNING_OLD_STYLE_CAST() \
_Pragma("GCC diagnostic pop")

#define DISABLE_WARNING_NARROWING_CONSTRUCTOR()
#define ENABLE_WARNING_NARROWING_CONSTRUCTOR()

#else

#define DISABLE_WARNING_FLOAT_EQUALITY()
#define ENABLE_WARNING_FLOAT_EQUALITY()

#define DISABLE_WARNING_OLD_STYLE_CAST()
#define ENABLE_WARNING_OLD_STYLE_CAST()

// msvc warns when I don't feel like adding .f to a bunch of literals

#define DISABLE_WARNING_NARROWING_CONSTRUCTOR() \
__pragma(warning(push)) \
__pragma(warning(disable : 4305))

#define ENABLE_WARNING_NARROWING_CONSTRUCTOR() \
__pragma(warning(pop))

#endif

//============================================================================//

#define SQEE_EXPAND(Arg) Arg // needed to work around an MSVC bug

#define SQEE_MACRO_OVERLOAD(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, \
                            _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                            _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                            _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                            _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                            Name, ...) Name

//============================================================================//

#define SQEE_FOR_EACH_01(Functor, Arg) Functor(Arg)
#define SQEE_FOR_EACH_02(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_01(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_03(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_02(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_04(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_03(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_05(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_04(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_06(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_05(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_07(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_06(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_08(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_07(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_09(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_08(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_10(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_09(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_11(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_10(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_12(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_11(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_13(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_12(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_14(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_13(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_15(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_14(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_16(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_15(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_17(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_16(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_18(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_17(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_19(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_18(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_20(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_19(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_21(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_20(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_22(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_21(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_23(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_22(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_24(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_23(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_25(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_24(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_26(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_25(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_27(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_26(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_28(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_27(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_29(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_28(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_30(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_29(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_31(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_30(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_32(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_31(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_33(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_32(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_34(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_33(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_35(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_34(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_36(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_35(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_37(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_36(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_38(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_37(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_39(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_38(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_40(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_39(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_41(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_40(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_42(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_41(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_43(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_42(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_44(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_43(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_45(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_44(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_46(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_45(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_47(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_46(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_48(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_47(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_49(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_48(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_50(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_49(Functor, __VA_ARGS__))

#define SQEE_FOR_EACH(Functor, ...) \
    SQEE_EXPAND ( SQEE_MACRO_OVERLOAD ( __VA_ARGS__, \
        SQEE_FOR_EACH_50, SQEE_FOR_EACH_49, SQEE_FOR_EACH_48, SQEE_FOR_EACH_47, SQEE_FOR_EACH_46, \
        SQEE_FOR_EACH_45, SQEE_FOR_EACH_44, SQEE_FOR_EACH_43, SQEE_FOR_EACH_42, SQEE_FOR_EACH_41, \
        SQEE_FOR_EACH_40, SQEE_FOR_EACH_39, SQEE_FOR_EACH_38, SQEE_FOR_EACH_37, SQEE_FOR_EACH_36, \
        SQEE_FOR_EACH_35, SQEE_FOR_EACH_34, SQEE_FOR_EACH_33, SQEE_FOR_EACH_32, SQEE_FOR_EACH_31, \
        SQEE_FOR_EACH_30, SQEE_FOR_EACH_29, SQEE_FOR_EACH_28, SQEE_FOR_EACH_27, SQEE_FOR_EACH_26, \
        SQEE_FOR_EACH_25, SQEE_FOR_EACH_24, SQEE_FOR_EACH_23, SQEE_FOR_EACH_22, SQEE_FOR_EACH_21, \
        SQEE_FOR_EACH_20, SQEE_FOR_EACH_19, SQEE_FOR_EACH_18, SQEE_FOR_EACH_17, SQEE_FOR_EACH_16, \
        SQEE_FOR_EACH_15, SQEE_FOR_EACH_14, SQEE_FOR_EACH_13, SQEE_FOR_EACH_12, SQEE_FOR_EACH_11, \
        SQEE_FOR_EACH_10, SQEE_FOR_EACH_09, SQEE_FOR_EACH_08, SQEE_FOR_EACH_07, SQEE_FOR_EACH_06, \
        SQEE_FOR_EACH_05, SQEE_FOR_EACH_04, SQEE_FOR_EACH_03, SQEE_FOR_EACH_02, SQEE_FOR_EACH_01, \
    ) ( Functor, __VA_ARGS__ ) )

//============================================================================//

#define SWITCH(Value) do { using SwitchValueT = std::decay_t<decltype(Value)>; switch (Value)

#define SWITCH_END } while (false)

#define SQEE_CASE_INNER(Case) case SwitchValueT::Case:

#define CASE(...) break; SQEE_FOR_EACH(SQEE_CASE_INNER, __VA_ARGS__)

#define CASE_DEFAULT break; default:

//============================================================================//

#define TRY_IF try { if

#define TRY_FOR try { for

#define TRY_WHILE try { while

#define CATCH } catch
