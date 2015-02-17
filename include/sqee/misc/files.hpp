#pragma once
#include "forward.hpp"

#include <vector>

#include "redist/jsoncpp.hpp"

namespace sq {

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

std::vector<std::vector<string>> get_words_from_file(const string& _path);

}
