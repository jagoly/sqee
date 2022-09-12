// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

//============================================================================//

#if defined(SQEE_GNU) || defined(SQEE_CLANG)

#define DISABLE_WARNING_FLOAT_EQUALITY() \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")

#define ENABLE_WARNING_FLOAT_EQUALITY() \
_Pragma("GCC diagnostic pop")

#define DISABLE_WARNING_OLD_STYLE_CAST() \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")

#define ENABLE_WARNING_OLD_STYLE_CAST() \
_Pragma("GCC diagnostic pop")

#elif defined(SQEE_MSVC)

#define DISABLE_WARNING_FLOAT_EQUALITY()
#define ENABLE_WARNING_FLOAT_EQUALITY()

#define DISABLE_WARNING_OLD_STYLE_CAST()
#define ENABLE_WARNING_OLD_STYLE_CAST()

#endif

//============================================================================//

#define SQEE_EXPAND(Arg) Arg // needed to work around an MSVC bug

#define SQEE_MACRO_OVERLOAD( \
  _00, _01, _02, _03, _04, _05, _06, _07, _08, _09, \
  _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
  _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
  _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
  _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
  _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
  _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, \
  _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, \
  _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, \
  _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, \
  Name, ...) Name

//============================================================================//

#define SQEE_FOR_EACH_00(Functor, Arg) Functor(Arg)
#define SQEE_FOR_EACH_01(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_00(Functor, __VA_ARGS__))
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
#define SQEE_FOR_EACH_51(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_50(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_52(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_51(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_53(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_52(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_54(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_53(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_55(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_54(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_56(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_55(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_57(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_56(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_58(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_57(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_59(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_58(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_60(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_59(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_61(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_60(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_62(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_61(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_63(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_62(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_64(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_63(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_65(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_64(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_66(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_65(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_67(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_66(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_68(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_67(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_69(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_68(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_70(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_69(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_71(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_70(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_72(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_71(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_73(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_72(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_74(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_73(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_75(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_74(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_76(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_75(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_77(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_76(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_78(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_77(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_79(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_78(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_80(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_79(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_81(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_80(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_82(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_81(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_83(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_82(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_84(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_83(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_85(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_84(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_86(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_85(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_87(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_86(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_88(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_87(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_89(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_88(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_90(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_89(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_91(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_90(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_92(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_91(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_93(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_92(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_94(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_93(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_95(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_94(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_96(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_95(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_97(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_96(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_98(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_97(Functor, __VA_ARGS__))
#define SQEE_FOR_EACH_99(Functor, Arg, ...) Functor(Arg) SQEE_EXPAND(SQEE_FOR_EACH_98(Functor, __VA_ARGS__))

#define SQEE_FOR_EACH(Functor, ...) SQEE_EXPAND ( SQEE_MACRO_OVERLOAD ( __VA_ARGS__, \
  SQEE_FOR_EACH_99, SQEE_FOR_EACH_98, SQEE_FOR_EACH_97, SQEE_FOR_EACH_96, SQEE_FOR_EACH_95, \
  SQEE_FOR_EACH_94, SQEE_FOR_EACH_93, SQEE_FOR_EACH_92, SQEE_FOR_EACH_91, SQEE_FOR_EACH_90, \
  SQEE_FOR_EACH_89, SQEE_FOR_EACH_88, SQEE_FOR_EACH_87, SQEE_FOR_EACH_86, SQEE_FOR_EACH_85, \
  SQEE_FOR_EACH_84, SQEE_FOR_EACH_83, SQEE_FOR_EACH_82, SQEE_FOR_EACH_81, SQEE_FOR_EACH_80, \
  SQEE_FOR_EACH_79, SQEE_FOR_EACH_78, SQEE_FOR_EACH_77, SQEE_FOR_EACH_76, SQEE_FOR_EACH_75, \
  SQEE_FOR_EACH_74, SQEE_FOR_EACH_73, SQEE_FOR_EACH_72, SQEE_FOR_EACH_71, SQEE_FOR_EACH_70, \
  SQEE_FOR_EACH_69, SQEE_FOR_EACH_68, SQEE_FOR_EACH_67, SQEE_FOR_EACH_66, SQEE_FOR_EACH_65, \
  SQEE_FOR_EACH_64, SQEE_FOR_EACH_63, SQEE_FOR_EACH_62, SQEE_FOR_EACH_61, SQEE_FOR_EACH_60, \
  SQEE_FOR_EACH_59, SQEE_FOR_EACH_58, SQEE_FOR_EACH_57, SQEE_FOR_EACH_56, SQEE_FOR_EACH_55, \
  SQEE_FOR_EACH_54, SQEE_FOR_EACH_53, SQEE_FOR_EACH_52, SQEE_FOR_EACH_51, SQEE_FOR_EACH_50, \
  SQEE_FOR_EACH_49, SQEE_FOR_EACH_48, SQEE_FOR_EACH_47, SQEE_FOR_EACH_46, SQEE_FOR_EACH_45, \
  SQEE_FOR_EACH_44, SQEE_FOR_EACH_43, SQEE_FOR_EACH_42, SQEE_FOR_EACH_41, SQEE_FOR_EACH_40, \
  SQEE_FOR_EACH_39, SQEE_FOR_EACH_38, SQEE_FOR_EACH_37, SQEE_FOR_EACH_36, SQEE_FOR_EACH_35, \
  SQEE_FOR_EACH_34, SQEE_FOR_EACH_33, SQEE_FOR_EACH_32, SQEE_FOR_EACH_31, SQEE_FOR_EACH_30, \
  SQEE_FOR_EACH_29, SQEE_FOR_EACH_28, SQEE_FOR_EACH_27, SQEE_FOR_EACH_26, SQEE_FOR_EACH_25, \
  SQEE_FOR_EACH_24, SQEE_FOR_EACH_23, SQEE_FOR_EACH_22, SQEE_FOR_EACH_21, SQEE_FOR_EACH_20, \
  SQEE_FOR_EACH_19, SQEE_FOR_EACH_18, SQEE_FOR_EACH_17, SQEE_FOR_EACH_16, SQEE_FOR_EACH_15, \
  SQEE_FOR_EACH_14, SQEE_FOR_EACH_13, SQEE_FOR_EACH_12, SQEE_FOR_EACH_11, SQEE_FOR_EACH_10, \
  SQEE_FOR_EACH_09, SQEE_FOR_EACH_08, SQEE_FOR_EACH_07, SQEE_FOR_EACH_06, SQEE_FOR_EACH_05, \
  SQEE_FOR_EACH_04, SQEE_FOR_EACH_03, SQEE_FOR_EACH_02, SQEE_FOR_EACH_01, SQEE_FOR_EACH_00, \
) ( Functor, __VA_ARGS__ ) )

//============================================================================//

#define SWITCH(Value) do { using SwitchValueT = std::decay_t<decltype(Value)>; switch (Value)

#define SWITCH_END } while (false)

#define SQEE_CASE_INNER(Case) case SwitchValueT::Case:

#define CASE(...) break; SQEE_FOR_EACH(SQEE_CASE_INNER, __VA_ARGS__)

#define CASE_DEFAULT break; default:
