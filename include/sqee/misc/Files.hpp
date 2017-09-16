#pragma once

#include <sqee/builtins.hpp>

namespace sq {

//============================================================================//

/// Check if a file exists
bool check_file_exists(const string& path);

/// Load a file into a simple string
string get_string_from_file(const string& path);

/// Load a file into a vector of bytes
std::vector<uchar> get_bytes_from_file(const string& path);

//============================================================================//

struct TokenisedFile
{
    using Tokens = std::vector<std::string_view>;
    struct Line { Tokens tokens; std::size_t num; };

    std::string fullString;
    std::vector<Line> lines;
};

/// Split a string into a vector of strings.
std::vector<string> tokenise_string(const string& str, char dlm);

/// Load a text file into an easy to parse structure.
TokenisedFile tokenise_file(const string& path);

//============================================================================//

/// Extract file name portion from a path
string file_name_from_path(const string& path);

/// Extract directory portion from a path
string directory_from_path(const string& path);

//============================================================================//

} // namespace sq
