#pragma once
#include <sqee/forward.hpp>

#include <vector>

#include <sqee/redist/jsoncpp.hpp>

namespace sq {

bool check_file_exists(const string& _path);

char get_file_first_byte(const string& _path);

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

std::vector<char> get_bytes_from_file(const string& _path);

std::vector<std::vector<string>> get_words_from_file(const string& _path);

}

inline uint stou(const string& _str, size_t* _idx = 0, int _base = 10) {
    return (uint)std::stoul(_str, _idx, _base);
} using std::stoi; using std::stof;
