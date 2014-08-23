#pragma once

#include <jsoncpp/json/json.h>

namespace sq {

Json::Value get_json_from_file(std::string _path);

std::string get_string_from_file(std::string _path);

}
