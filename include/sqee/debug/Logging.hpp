#pragma once

#include <sqee/builtins.hpp>

#include <sqee/misc/StringCast.hpp>

#include <sqee/redist/tinyformat.hpp>

namespace sq {

//============================================================================//

/// Get the system time in a tidy format.
string get_time_string();

//============================================================================//

/// Log an unformatted string without any prefix.
inline void log_only(const string& str)
{
    std::cout << str << std::endl;
}

/// Log a formatted string without any prefix.
template <class... Args>
inline void log_only(const char* fmt, const Args&... args)
{
    log_only(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted INFO message.
inline void log_info(const string& str)
{
    std::cout << get_time_string() << " INFO: ";
    std::cout << str << std::endl;
}

/// Log a formatted INFO message.
template <class... Args>
inline void log_info(const char* fmt, const Args&... args)
{
    log_info(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted WARNING message.
inline void log_warning(const string& str)
{
    std::cout << get_time_string() << " WARNING: ";
    std::cout << str << std::endl;
}

/// Log a formatted WARNING message.
template <class... Args>
inline void log_warning(const char* fmt, const Args&... args)
{
    log_warning(tfm::format(fmt, args...));
}

//============================================================================//

/// Log an unformatted ERROR message, then abort.
inline void log_error(const string& str)
{
    std::cout << get_time_string() << " ERROR: ";
    std::cout << str << std::endl; std::abort();
}

/// Log a formatted ERROR message, then abort.
template <class... Args>
inline void log_error(const char* fmt, const Args&... args)
{
    log_error(tfm::format(fmt, args...));
}

//============================================================================//

/// If not condition, log an unformatted ERROR message, then abort.
inline void log_assert(bool condition, const string& str)
{
    if (condition == true) return;

    std::cout << get_time_string() << " ERROR: ";
    std::cout << str << std::endl; std::abort();
}

/// If not condition, log a formatted ERROR message, then abort.
template <class... Args>
inline void log_assert(bool condition, const char* fmt, const Args&... args)
{
    log_assert(condition, tfm::format(fmt, args...));
}

//============================================================================//

} // namespace sq
