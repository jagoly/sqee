#pragma once

#include <redist/jsoncpp.hpp>
#include <defs.hpp>

namespace sq {

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

vector<vector<string>> get_words_from_file(const string& _path);

}
