// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <cstdio>

// Note that all file paths in sqee are unix style, don't try to use dos paths.
// Windows is kind enough to work just fine with unix paths, so there's no need
// for anything platform specific.

// todo: in general, file loading in sqee is pretty bad.
//  - compute_resource_path requires opening and closing files an extra time
//  - check_file_exists requires opening and closing a file an extra time

namespace sq {

//============================================================================//

/// Check if a file exists.
SQEE_API bool check_file_exists(const String& path);

/// Load a text file into a string.
SQEE_API String get_string_from_file(const String& path);

/// If it exists, load a text file into a string.
SQEE_API std::optional<String> try_get_string_from_file(const String& path);

/// Load a binary file into a vector of bytes.
SQEE_API std::vector<std::byte> get_bytes_from_file(const String& path);

/// Save a string to a text file.
SQEE_API void save_string_to_file(const String& path, StringView str);

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

//============================================================================//

/// Compute the path to a file using its key, or just the key if none found.
///
/// @param key        the path with no prefix or extension
/// @param prefixes   directories to search in, highest priority first
/// @param extensions extensions to check, hightet priority first
///
SQEE_API String compute_resource_path(StringView key,
                                      std::initializer_list<StringView> prefixes,
                                      std::initializer_list<StringView> extensions);

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
