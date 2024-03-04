// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

//============================================================================//

#ifdef SQEE_DEBUG

#include <sqee/debug/Logging.hpp>

#if defined(SQEE_GNU) || defined(SQEE_CLANG)

#define SQASSERT(Condition, Message) \
do { \
  if (!(Condition)) \
    sq::log_error_multiline ( \
      "assertion failed: ( {} )\nfile '{}', line {}\n{}\n{}", \
      #Condition, __FILE__, __LINE__, __PRETTY_FUNCTION__, Message \
    ); \
} while (false)

#define SQEE_UNREACHABLE() \
do { \
  sq::log_error_multiline ( \
    "unreachable code reached\nfile '{}', line {}\n{}", \
    __FILE__, __LINE__, __PRETTY_FUNCTION__ \
  ); \
} while (false)

#elif defined(SQEE_MSVC)

#define SQASSERT(Condition, Message) \
do { \
  if (!(Condition)) \
    sq::log_error_multiline ( \
      "assertion failed: ( {} )\nfile '{}', line {}\n{}\n{}", \
      #Condition, __FILE__, __LINE__, __FUNCSIG__, Message \
    ); \
} while (false)

#define SQEE_UNREACHABLE() \
do { \
  sq::log_error_multiline ( \
    "unreachable code reached\nfile '{}', line {}\n{}", \
    __FILE__, __LINE__, __FUNCSIG__ \
  ); \
} while (false)

#endif

//============================================================================//

#else

#define SQASSERT(Condition, Message) do { (void)sizeof(Condition); } while (false)

#if defined(SQEE_GNU) || defined(SQEE_CLANG)
  #define SQEE_UNREACHABLE() __builtin_unreachable()
#elif defined(SQEE_MSVC)
  #define SQEE_UNREACHABLE() __assume(0)
#endif

#endif
