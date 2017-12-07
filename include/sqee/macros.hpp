#pragma once

//============================================================================//

#define SQEE_MACRO_OVERLOAD(_1, _2, _3, _4, _5, _6, _7, _8, Name, ...) Name

#define SQEE_CONCAT(A, B) A ## B

#define SQEE_MSVC_FIX(Arg) Arg

//============================================================================//

#define WITH(Expr) if ([[maybe_unused]] const auto WithVarName = Expr; true)

#define WITH_IF(Expr) if (const auto WithVarName = Expr; bool(WithVarName) == true)

//============================================================================//

#define SWITCH(Value) { using SwitchValueT = decltype(Value); switch (Value)

#define SWITCH_END }

#define SQEE_CASE_1(Case) case SwitchValueT::Case:
#define SQEE_CASE_2(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_1(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_3(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_2(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_4(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_3(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_5(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_4(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_6(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_5(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_7(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_6(__VA_ARGS__) ) case SwitchValueT::Case:
#define SQEE_CASE_8(Case, ...) SQEE_MSVC_FIX ( SQEE_CASE_7(__VA_ARGS__) ) case SwitchValueT::Case:

#define CASE(...) break; \
    SQEE_MSVC_FIX ( SQEE_MACRO_OVERLOAD ( __VA_ARGS__, \
        SQEE_CASE_8, SQEE_CASE_7, SQEE_CASE_6, SQEE_CASE_5, SQEE_CASE_4, SQEE_CASE_3, SQEE_CASE_2, SQEE_CASE_1 \
    ) ( __VA_ARGS__ ) )

#define CASE_DEFAULT break; default:

//============================================================================//

#define SQEE_ENUM_TO_STRING_CASE(Case) case EnumType::Case: return #Case;

#define SQEE_ENUM_TO_STRING_BLOCK_BEGIN(Type) \
constexpr const char* enum_to_string(Type value) \
{ using EnumType = Type; switch (value) {

#define SQEE_ENUM_TO_STRING_BLOCK_END \
} /*throw std::runtime_error("invalid enum");*/ }

// the above SHOULD be valid, but doesn't work in GCC
