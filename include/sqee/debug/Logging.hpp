// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <fmt/core.h> // IWYU pragma: export

namespace sq {

//============================================================================//

namespace detail {

SQEE_API void log_time_info();
SQEE_API void log_time_warning();
SQEE_API void log_time_error();
SQEE_API void log_time_debug();

SQEE_API void log_multiline(StringView str);

} // namespace detail

//============================================================================//

/// Write a formatted string to the log.
template <class... Args>
inline void log_raw(StringView str, const Args&... args)
{
    if constexpr (sizeof...(args) == 0) std::fwrite(str.data(), 1u, str.size(), stdout);
    else fmt::print(stdout, str, args...);

    std::fputc('\n', stdout);
    std::fflush(stdout);
}

/// Write a formatted multiline string to the log.
template <class... Args>
inline void log_raw_multiline(StringView str, const Args&... args)
{
    if constexpr (sizeof...(args) == 0) detail::log_multiline(str);
    else detail::log_multiline(fmt::format(str, args...));
}

//============================================================================//

/// Log a formatted INFO message.
template <class... Args>
inline void log_info(StringView str, const Args&... args)
{
    detail::log_time_info();
    log_raw(str, args...);
}

/// Log a formatted multiline INFO message.
template <class... Args>
inline void log_info_multiline(StringView str, const Args&... args)
{
    detail::log_time_info();
    log_raw_multiline(str, args...);
}

//============================================================================//

/// Log a formatted WARNING message.
template <class... Args>
inline void log_warning(StringView str, const Args&... args)
{
    detail::log_time_warning();
    log_raw(str, args...);
}

/// Log a formatted multiline WARNING message.
template <class... Args>
inline void log_warning_multiline(StringView str, const Args&... args)
{
    detail::log_time_warning();
    log_raw_multiline(str, args...);
}

//============================================================================//

/// Log a formatted ERROR message, then abort.
template <class... Args>
inline void log_error(StringView str, const Args&... args)
{
    detail::log_time_error();
    log_raw(str, args...);
    std::abort();
}

/// Log a formatted multiline ERROR message, then abort.
template <class... Args>
inline void log_error_multiline(StringView str, const Args&... args)
{
    detail::log_time_error();
    log_raw_multiline(str, args...);
    std::abort();
}

//============================================================================//

/// If in debug mode, log a formatted DEBUG message.
template <class... Args>
inline void log_debug(StringView str, const Args&... args)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw(str, args...);
  #else
    (void)str;
    ((void)args, ...);
  #endif
}

/// If in debug mode, log a formatted multiline DEBUG message.
template <class... Args>
inline void log_debug_multiline(StringView str, const Args&... args)
{
  #ifdef SQEE_DEBUG
    detail::log_time_debug();
    log_raw_multiline(str, args...);
  #else
    (void)str;
    ((void)args, ...);
  #endif
}

//============================================================================//

} // namespace sq
