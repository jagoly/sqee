#pragma once

#include <redist/jsoncpp.hpp>
#include <defs.hpp>

namespace sq {

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

}
