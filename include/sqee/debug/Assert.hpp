// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

//============================================================================//

#ifdef SQEE_DEBUG

#include <sqee/debug/Logging.hpp>

#define SQASSERT(Condition, Message) \
    do { \
        if (!(Condition)) \
        { \
            sq::log_error_multiline("assertion failed: ( {} )\nfile '{}', line {}\n{}", #Condition, __FILE__, __LINE__, Message); \
        } \
    } while (false)

#else

#define SQASSERT(Condition, Message) do {} while (void(Condition), false)

#endif
