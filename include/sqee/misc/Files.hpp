#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

inline uint stou(const string& _str, size_t* _idx = 0, int _base = 10) {
    return uint(std::stoul(_str, _idx, _base));
} using std::stoi; using std::stof;

inline Vec2F svtofv2(const vector<string>& _vec, uint _pos) {
    return Vec2F(stof(_vec[_pos+0]), stof(_vec[_pos+1])); }
inline Vec2I svtoiv2(const vector<string>& _vec, uint _pos) {
    return Vec2I(stoi(_vec[_pos+0]), stoi(_vec[_pos+1])); }
inline Vec2U svtouv2(const vector<string>& _vec, uint _pos) {
    return Vec2U(stou(_vec[_pos+0]), stou(_vec[_pos+1])); }
inline Vec3F svtofv3(const vector<string>& _vec, uint _pos) {
    return Vec3F(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2])); }
inline Vec3I svtoiv3(const vector<string>& _vec, uint _pos) {
    return Vec3I(stoi(_vec[_pos+0]), stoi(_vec[_pos+1]), stoi(_vec[_pos+2])); }
inline Vec3U svtouv3(const vector<string>& _vec, uint _pos) {
    return Vec3U(stou(_vec[_pos+0]), stou(_vec[_pos+1]), stou(_vec[_pos+2])); }
inline Vec4F svtofv4(const vector<string>& _vec, uint _pos) {
    return Vec4F(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2]), stof(_vec[_pos+3])); }
inline Vec4I svtoiv4(const vector<string>& _vec, uint _pos) {
    return Vec4I(stoi(_vec[_pos+0]), stoi(_vec[_pos+1]), stoi(_vec[_pos+2]), stoi(_vec[_pos+3])); }
inline Vec4U svtouv4(const vector<string>& _vec, uint _pos) {
    return Vec4U(stou(_vec[_pos+0]), stou(_vec[_pos+1]), stou(_vec[_pos+2]), stou(_vec[_pos+3])); }
inline QuatF svtofqt(const vector<string>& _vec, uint _pos) {
    return QuatF(stof(_vec[_pos+1]), stof(_vec[_pos+2]), stof(_vec[_pos+3]), stof(_vec[_pos+0])); }
inline QuatF svtofeu(const vector<string>& _vec, uint _pos) {
    return QuatF(stof(_vec[_pos+0]), stof(_vec[_pos+1]), stof(_vec[_pos+2])); }

namespace sq {

/// Check if a file exists
bool check_file_exists(const string& _path);

/// Get the first byte/char of a file
char get_file_first_char(const string& _path);

/// Load a file into a simple string
string get_string_from_file(const string& _path);

/// Load a file into a vector of bytes
vector<uchar> get_bytes_from_file(const string& _path);

/// Tokenise a string into a vector of strings
vector<string> tokenise_string(const string& _str, char _dlm);

/// Tokenise a file and remove empty lines, also get line numbers
vector<pair<vector<string>, uint>> tokenise_file(const string& _path);

/// Get a list of files in a directory
vector<string> get_files_from_dir(const string& _path);

/// Extract file name portion from a path
string file_name_from_path(const string& _path);

/// Extract directory portion from a path
string directory_from_path(const string& _path);

}
