#pragma once

#include <sqee/builtins.hpp>
#include <sqee/redist/tinyformat.hpp>

namespace sq {

/// Get the system time in a tidy format
string get_time_string();

template<typename... Args>
/// Log a formatted string without any prefix
void log_only(const string& _fmt, const Args&... _args) {
    tfm::format(std::cout, _fmt.c_str(), _args...);
    std::cout << std::endl;
}

template<typename... Args>
/// Log a formatted INFO message
void log_info(const string& _fmt, const Args&... _args) {
    std::cout << get_time_string() << " INFO: ";
    log_only(_fmt, _args...);
}

template<typename... Args>
/// Log a formatted WARNING message
void log_warning(const string& _fmt, const Args&... _args) {
    std::cout << get_time_string() << " WARNING: ";
    log_only(_fmt, _args...);
}

template<typename... Args>
/// Log a formatted ERROR message, then abort
void log_error(const string& _fmt, const Args&... _args) {
    std::cout << get_time_string() << " ERROR: ";
    log_only(_fmt, _args...); std::abort();
}

template<typename... Args>
/// If condition is false, log a formatted ERROR message, then abort
void log_assert(bool _condition, const string& _fmt, const Args&... _args) {
    if (_condition == true) return;
    std::cout << get_time_string() << " ERROR: ";
    log_only(_fmt, _args...); std::abort();
}

}
