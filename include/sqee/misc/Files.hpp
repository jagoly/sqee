#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/jsoncpp.hpp>

inline uint stou(const string& _str, size_t* _idx = 0, int _base = 10) {
    return (uint)std::stoul(_str, _idx, _base);
} using std::stoi; using std::stof;

inline fvec2 svtofv2(const vector<string>& _vec, uint _pos) {
    return fvec2(stof(_vec[_pos+0]), stof(_vec[_pos+1])); }
inline ivec2 svtoiv2(const vector<string>& _vec, uint _pos) {
    return ivec2(stoi(_vec[_pos+0]), stoi(_vec[_pos+1])); }
inline uvec2 svtouv2(const vector<string>& _vec, uint _pos) {
    return ivec2(stou(_vec[_pos+0]), stou(_vec[_pos+1])); }
inline fvec3 svtofv3(const vector<string>& _vec, uint _pos) {
    return fvec3(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2])); }
inline ivec3 svtoiv3(const vector<string>& _vec, uint _pos) {
    return ivec3(stoi(_vec[_pos+0]), stoi(_vec[_pos+1]), stoi(_vec[_pos+2])); }
inline uvec3 svtouv3(const vector<string>& _vec, uint _pos) {
    return ivec3(stou(_vec[_pos+0]), stou(_vec[_pos+1]), stou(_vec[_pos+2])); }
inline fvec4 svtofv4(const vector<string>& _vec, uint _pos) {
    return fvec4(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2]), stof(_vec[_pos+3])); }
inline ivec4 svtoiv4(const vector<string>& _vec, uint _pos) {
    return ivec4(stoi(_vec[_pos+0]), stoi(_vec[_pos+1]), stoi(_vec[_pos+2]), stoi(_vec[_pos+3])); }
inline uvec4 svtouv4(const vector<string>& _vec, uint _pos) {
    return ivec4(stou(_vec[_pos+0]), stou(_vec[_pos+1]), stou(_vec[_pos+2]), stou(_vec[_pos+3])); }
inline fquat svtofqt(const vector<string>& _vec, uint _pos) {
    return fquat(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2]), stof(_vec[_pos+3])); }
inline fquat svtofeu(const vector<string>& _vec, uint _pos) {
    return fquat(glm::radians(fvec3(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2])))); }

namespace sq {

bool check_file_exists(const string& _path);

char get_file_first_byte(const string& _path);

Json::Value get_json_from_file(const string& _path);

string get_string_from_file(const string& _path);

vector<char> get_bytes_from_file(const string& _path);

vector<vector<string>> get_words_from_file(const string& _path);

vector<string> get_files_from_dir(const string& _path);

}
