#pragma once

#include <sqee/builtins.hpp>

#include <sqee/misc/StringCast.hpp>

#include <sqee/redist/tinyformat.hpp>

namespace sq {

//============================================================================//

/// Get the system time in a tidy format.
string get_time_string();

//============================================================================//

/// Log a formatted string without any prefix.
template <class... Args>
void log_only(const string& fmt, const Args&... args)
{
    tfm::format(std::cout, fmt.c_str(), args...);
    std::cout << std::endl;
}

/// Log a formatted INFO message.
template <class... Args>
void log_info(const string& fmt, const Args&... args)
{
    std::cout << get_time_string() << " INFO: ";
    tfm::format(std::cout, fmt.c_str(), args...);
    std::cout << std::endl;
}

/// Log a formatted WARNING message.
template <class... Args>
void log_warning(const string& fmt, const Args&... args)
{
    std::cout << get_time_string() << " WARNING: ";
    tfm::format(std::cout, fmt.c_str(), args...);
    std::cout << std::endl;
}

/// Log a formatted ERROR message, then abort.
template <class... Args>
void log_error(const string& fmt, const Args&... args)
{
    std::cout << get_time_string() << " ERROR: ";
    tfm::format(std::cout, fmt.c_str(), args...);
    std::cout << std::endl; std::abort();
}

/// If not condition, log a formatted ERROR message, then abort.
template <class... Args>
void log_assert(bool condition, const string& fmt, const Args&... args)
{
    if (condition == true) return;

    std::cout << get_time_string() << " ERROR: ";
    tfm::format(std::cout, fmt.c_str(), args...);
    std::cout << std::endl; std::abort();
}

//============================================================================//

} // namespace sq
