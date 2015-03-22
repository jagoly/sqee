#pragma once
#include "forward.hpp"

#include <vector>

#include "redist/jsoncpp.hpp"

namespace sq {

bool check_file_exists(const string& _path);

char get_file_first_byte(const string& _path);

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

std::vector<char> get_bytes_from_file(const string& _path);

std::vector<std::vector<string>> get_words_from_file(const string& _path);

}
