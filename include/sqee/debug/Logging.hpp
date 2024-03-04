// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/export.hpp>

#include <fmt/format.h>

#include <cstdlib>
#include <string_view>

namespace sq {

//============================================================================//

namespace detail {

SQEE_API void log(std::string_view category, std::string_view str);

SQEE_API void log_multiline(std::string_view category, std::string_view str);

SQEE_API void log(std::string_view category, fmt::string_view fstr, fmt::format_args args);

SQEE_API void log_multiline(std::string_view category, fmt::string_view fstr, fmt::format_args args);

} // namespace detail

//============================================================================//

inline void log_info(std::string_view str)
{
    detail::log("INFO", str);
}

inline void log_info_multiline(std::string_view str)
{
    detail::log_multiline("INFO", str);
}

template <class... Args>
inline void log_info(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log("INFO", fstr, fmt::make_format_args(args...));
}

template <class... Args>
inline void log_info_multiline(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log_multiline("INFO", fstr, fmt::make_format_args(args...));
}

//============================================================================//

inline void log_warning(std::string_view str)
{
    detail::log("WARNING", str);
}

inline void log_warning_multiline(std::string_view str)
{
    detail::log_multiline("WARNING", str);
}

template <class... Args>
inline void log_warning(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log("WARNING", fstr, fmt::make_format_args(args...));
}

template <class... Args>
inline void log_warning_multiline(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log_multiline("WARNING", fstr, fmt::make_format_args(args...));
}

//============================================================================//

[[noreturn]] inline void log_error(std::string_view str)
{
    detail::log("ERROR", str);
    std::abort();
}

[[noreturn]] inline void log_error_multiline(std::string_view str)
{
    detail::log_multiline("ERROR", str);
    std::abort();
}

template <class... Args>
[[noreturn]] inline void log_error(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log("ERROR", fstr, fmt::make_format_args(args...));
    std::abort();
}

template <class... Args>
[[noreturn]] inline void log_error_multiline(fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log_multiline("ERROR", fstr, fmt::make_format_args(args...));
    std::abort();
}

//============================================================================//

inline void log_debug(std::string_view str)
{
  #ifdef SQEE_DEBUG
    detail::log("DEBUG", str);
  #else
    (void)str;
  #endif
}

inline void log_debug_multiline(std::string_view str)
{
  #ifdef SQEE_DEBUG
    detail::log_multiline("DEBUG", str);
  #else
    (void)str;
  #endif
}

template <class... Args>
inline void log_debug(fmt::format_string<Args...> fstr, Args&&... args)
{
  #ifdef SQEE_DEBUG
    detail::log("DEBUG", fstr, fmt::make_format_args(args...));
  #else
    (void)fstr; ((void)args, ...);
  #endif
}

template <class... Args>
inline void log_debug_multiline(fmt::format_string<Args...> fstr, Args&&... args)
{
  #ifdef SQEE_DEBUG
    detail::log_multiline("DEBUG", fstr, fmt::make_format_args(args...));
  #else
    (void)fstr; ((void)args, ...);
  #endif
}

//============================================================================//

inline void log_custom(std::string_view category, std::string_view str)
{
    detail::log(category, str);
}

inline void log_custom_multiline(std::string_view category, std::string_view str)
{
    detail::log_multiline(category, str);
}

template <class... Args>
inline void log_custom(std::string_view category, fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log(category, fstr, fmt::make_format_args(args...));
}

template <class... Args>
inline void log_custom_multiline(std::string_view category, fmt::format_string<Args...> fstr, Args&&... args)
{
    detail::log_multiline(category, fstr, fmt::make_format_args(args...));
}

//============================================================================//

} // namespace sq
