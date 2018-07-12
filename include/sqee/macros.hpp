#pragma once

//============================================================================//

#define SQEE_MACRO_OVERLOAD(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, Name, ...) Name

#define SQEE_CONCAT(A, B) A ## B

#define SQEE_MSVC_FIX(Arg) Arg

//============================================================================//

#define SWITCH(Value) { using SwitchValueT = std::decay_t<decltype(Value)>; switch (Value)

#define SWITCH_END }

#define SQEE_CASE_01(Case) case SwitchValueT::Case:
#define SQEE_CASE_02(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_01(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_03(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_02(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_04(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_03(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_05(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_04(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_06(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_05(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_07(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_06(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_08(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_07(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_09(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_08(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_10(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_09(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_11(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_10(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_12(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_11(__VA_ARGS__) ) case SwitchValueT::Case:

#define CASE(...) break; \
    SQEE_MSVC_FIX ( SQEE_MACRO_OVERLOAD ( __VA_ARGS__, \
        SQEE_CASE_12, SQEE_CASE_11, SQEE_CASE_10, SQEE_CASE_09, SQEE_CASE_08, SQEE_CASE_07, \
        SQEE_CASE_06, SQEE_CASE_05, SQEE_CASE_04, SQEE_CASE_03, SQEE_CASE_02, SQEE_CASE_01 \
    ) ( __VA_ARGS__ ) )

#define CASE_DEFAULT break; default:

//============================================================================//

#define SQEE_ENUM_TO_STRING_CASE(Case) case EnumType::Case: return #Case;

#define SQEE_ENUM_TO_STRING_BLOCK_BEGIN(Type) \
constexpr const char* enum_to_string(Type value) \
{ using EnumType = Type; switch (value) {

#define SQEE_ENUM_TO_STRING_BLOCK_END \
default: throw std::runtime_error("invalid enum"); } }
