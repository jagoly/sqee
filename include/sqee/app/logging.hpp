#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/tinyformat.hpp>

namespace sq {

string get_time_str();

template<typename... Args>
void log_only(const string& _fmt, const Args&... _args) {
    tfm::format(std::cout, _fmt.c_str(), _args...);
    std::cout << std::endl;
}

template<typename... Args>
void log_info(const string& _fmt, const Args&... _args) {
    std::cout << get_time_str() << " INFO: ";
    log_only(_fmt, _args...);
}

template<typename... Args>
void log_warning(const string& _fmt, const Args&... _args) {
    std::cout << get_time_str() << " WARNING: ";
    log_only(_fmt, _args...);
}

template<typename... Args>
void log_error(const string& _fmt, const Args&... _args) {
    std::cout << get_time_str() << " ERROR: ";
    log_only(_fmt, _args...);
}

}
