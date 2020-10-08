// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

//============================================================================//

#ifdef SQEE_DEBUG

#include <sqee/debug/Logging.hpp>

//============================================================================//

#if defined(SQEE_GNU) || defined(SQEE_CLANG)

#define SQASSERT(Condition, Message) \
do { if (!(Condition)) \
    sq::log_error_multiline("assertion failed: ( {} )\nfile '{}', line {}\n{}\n{}", #Condition, __FILE__, __LINE__, __PRETTY_FUNCTION__, Message); \
} while (false)

#elif defined(SQEE_MSVC)

#define SQASSERT(Condition, Message) \
do { if (!(Condition)) \
    sq::log_error_multiline("assertion failed: ( {} )\nfile '{}', line {}\n{}\n{}", #Condition, __FILE__, __LINE__, __FUNCSIG__, Message); \
} while (false)

#endif

//============================================================================//

#else

#define SQASSERT(Condition, Message) do {} while (void(Condition), false)

#endif
