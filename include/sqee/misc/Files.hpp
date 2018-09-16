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

struct TokenisedFile : sq::MoveOnly
{
    using Tokens = std::vector<string_view>;
    struct Line { Tokens tokens; size_t num; };

    string fullString;
    std::vector<Line> lines;
};

/// Load a text file into an easy to parse structure.
TokenisedFile tokenise_file(const string& path);

//============================================================================//

/// Extract file portion from a path
string_view file_from_path(const string& path);

/// Extract directory portion from a path
string_view directory_from_path(const string& path);

/// Extract the file extension from a path
string_view extension_from_path(const string& path);

//============================================================================//

inline string build_path(string path) { return path; }

template <class Arg, class... Args> inline
string build_path(string path, Arg&& arg, Args&&... args)
{
    if (!path.empty() && path.back() != '/') path.push_back('/');
    return build_path(path.append(arg), args...);
}

template <class... Args> inline
string build_path(Args&&... args) { return build_path(string(), args...); }

//============================================================================//

} // namespace sq
