#pragma once

#include <sqee/builtins.hpp>

//============================================================================//

inline uint stou(const string& str, size_t* idx = 0, int base = 10)
{ return uint(std::stoul(str, idx, base)); }

using std::stoi; using std::stof;

//============================================================================//

namespace sq {

//============================================================================//

/// Check if a file exists
bool check_file_exists(const string& path);

/// Get the first byte/char of a file
char get_file_first_char(const string& path);

/// Load a file into a simple string
string get_string_from_file(const string& path);

/// Load a file into a vector of bytes
std::vector<uchar> get_bytes_from_file(const string& path);

/// Tokenise a string into a vector of strings
std::vector<string> tokenise_string(const string& str, char dlm);

/// Tokenise a file and remove empty lines, also get line numbers
std::vector<std::pair<std::vector<string>, uint>> tokenise_file(const string& path);

/// Get a list of files in a directory
std::vector<string> get_files_from_dir(const string& path);

/// Extract file name portion from a path
string file_name_from_path(const string& path);

/// Extract directory portion from a path
string directory_from_path(const string& path);

//============================================================================//

} // namespace sq
