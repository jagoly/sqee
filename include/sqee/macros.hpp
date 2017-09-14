#pragma once

//============================================================================//

#define SQ_MACRO_OVERLOAD(_1, _2, _3, _4, _5, _6, _7, _8, Name, ...) Name

#define SQ_MSVC_FIX(Arg) Arg

#define SWITCH(Value) { using _MultiSwitchT = decltype(Value); switch (Value)

#define SWITCH_END }

#define CASE_1(Case) case _MultiSwitchT::Case:
#define CASE_2(Case, ...) SQ_MSVC_FIX ( CASE_1(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_3(Case, ...) SQ_MSVC_FIX ( CASE_2(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_4(Case, ...) SQ_MSVC_FIX ( CASE_3(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_5(Case, ...) SQ_MSVC_FIX ( CASE_4(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_6(Case, ...) SQ_MSVC_FIX ( CASE_5(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_7(Case, ...) SQ_MSVC_FIX ( CASE_6(__VA_ARGS__) ) case _MultiSwitchT::Case:
#define CASE_8(Case, ...) SQ_MSVC_FIX ( CASE_7(__VA_ARGS__) ) case _MultiSwitchT::Case:

#define CASE(...) break; \
    SQ_MSVC_FIX ( SQ_MACRO_OVERLOAD ( __VA_ARGS__, \
        CASE_8, CASE_7, CASE_6, CASE_5, CASE_4, CASE_3, CASE_2, CASE_1 \
    ) ( __VA_ARGS__ ) )

#define CASE_DEFAULT break; default:
