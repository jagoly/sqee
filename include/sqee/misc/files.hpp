#pragma once

#include <redist/jsoncpp.hpp>

namespace sq {

Json::Value get_json_from_file(const std::string& _path);

std::string get_string_from_file(const std::string& _path);

}
