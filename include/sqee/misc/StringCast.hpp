#pragma once

#include <string>

#include <sqee/setup.hpp>

namespace sq {

/// Convert to string with ChaiScript formatting
template<class T> std::string chai_string(const T& _val);

/// Convert to string with GLSL formatting
template<class T> std::string glsl_string(const T& _val);

}
