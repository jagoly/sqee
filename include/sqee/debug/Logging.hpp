// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/export.hpp>

#include <fmt/format.h>

#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace sq {

//============================================================================//

namespace detail {

SQEE_API void log_time_info();
SQEE_API void log_time_warning();
SQEE_API void log_time_error();
SQEE_API void log_time_debug();

} // namespace detail

//============================================================================//

SQEE_API void log_raw(std::string_view str);

SQEE_API void log_raw_multiline(std::string_view str);

template <class... Args>
inline void log_raw(fmt::format_string<Args...> str, Args&&... args)
{
    fmt::print(stdout, str, std::forward<Args>(args)...);
    std::fputc('\n', stdout);
    std::fflush(stdout);
}

template <class... Args>
inline void log_raw_multiline(fmt::format_string<Args...> str, Args&&... args)
{
    log_raw_multiline (
        fmt::format(str, std::forward<Args>(args)...)
    );
}

//============================================================================//

inline void log_info(std::string_view str)
{
    detail::log_time_info();
    log_raw(str);
}

inline void log_info_multiline(std::string_view str)
{
    detail::log_time_info();
    log_raw_multiline(str);
}

template <class... Args>
inline void log_info(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_info();
    log_raw(str, std::forward<Args>(args)...);
}

template <class... Args>
inline void log_info_multiline(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_info();
    log_raw_multiline(str, std::forward<Args>(args)...);
}

//============================================================================//

inline void log_warning(std::string_view str)
{
    detail::log_time_warning();
    log_raw(str);
}

inline void log_warning_multiline(std::string_view str)
{
    detail::log_time_warning();
    log_raw_multiline(str);
}

template <class... Args>
inline void log_warning(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_warning();
    log_raw(str, std::forward<Args>(args)...);
}

template <class... Args>
inline void log_warning_multiline(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_warning();
    log_raw_multiline(str, std::forward<Args>(args)...);
}

//============================================================================//

[[noreturn]] inline void log_error(std::string_view str)
{
    detail::log_time_error();
    log_raw(str);
    std::abort();
}

[[noreturn]] inline void log_error_multiline(std::string_view str)
{
    detail::log_time_error();
    log_raw_multiline(str);
    std::abort();
}

template <class... Args>
[[noreturn]] inline void log_error(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_error();
    log_raw(str, std::forward<Args>(args)...);
    std::abort();
}

template <class... Args>
[[noreturn]] inline void log_error_multiline(fmt::format_string<Args...> str, Args&&... args)
{
    detail::log_time_error();
    log_raw_multiline(str, std::forward<Args>(args)...);
    std::abort();
}

//============================================================================//

inline void log_debug(std::string_view str)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw(str);
  #else
    (void)str; ((void)args, ...);
  #endif
}

inline void log_debug_multiline(std::string_view str)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw_multiline(str);
  #else
    (void)str; ((void)args, ...);
  #endif
}

template <class... Args>
inline void log_debug(fmt::format_string<Args...> str, Args&&... args)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw(str, std::forward<Args>(args)...);
  #else
    (void)str; ((void)args, ...);
  #endif
}

template <class... Args>
inline void log_debug_multiline(fmt::format_string<Args...> str, Args&&... args)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw_multiline(str, std::forward<Args>(args)...);
  #else
    (void)str; ((void)args, ...);
  #endif
}

//============================================================================//

} // namespace sq
