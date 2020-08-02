// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

/// Check if a file exists
SQEE_API bool check_file_exists(const String& path);

/// Load a file into a string
SQEE_API String get_string_from_file(const String& path);

/// Save a string to a file
SQEE_API void save_string_to_file(const String& path, StringView str);

/// Load a file into a vector of bytes
SQEE_API std::vector<std::byte> get_bytes_from_file(const String& path);

//============================================================================//

struct TokenisedFile : private sq::MoveOnly
{
    using Tokens = std::vector<StringView>;
    struct Line { Tokens tokens; size_t num; };

    String fullString;
    std::vector<Line> lines;
};

/// Load a text file into an easy to parse structure
SQEE_API TokenisedFile tokenise_file(const String& path);

//============================================================================//

/// Extract file portion from a path
SQEE_API StringView file_from_path(StringView path);

/// Extract directory portion from a path
SQEE_API StringView directory_from_path(StringView path);

/// Extract the file extension from a path
SQEE_API StringView extension_from_path(StringView path);

/// Compute the full path to an existing file, nullopt if none found
SQEE_API std::optional<String> compute_resource_path(StringView base, StringView path, std::vector<StringView> extensions);

//============================================================================//

inline String build_path(String path) { return path; }

template <class Arg, class... Args> inline
String build_path(String path, Arg&& arg, Args&&... args)
{
    if (!path.empty() && path.back() != '/') path.push_back('/');
    return build_path(path.append(arg), args...);
}

template <class... Args> inline
String build_path(Args&&... args) { return build_path(String(), args...); }

//============================================================================//

} // namespace sq
