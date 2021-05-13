// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

// Note that all file paths in sqee are unix style, don't try to use dos paths.
// Windows is kind enough to work just fine with unix paths, so there's no need
// for anything platform specific.

namespace sq {

//============================================================================//

/// Load a text file into a string.
SQEE_API String read_text_from_file(const String& path);

/// Load a binary file into a vector of bytes.
SQEE_API std::vector<std::byte> read_bytes_from_file(const String& path);

/// If it exists, load a text file into a string.
SQEE_API std::optional<String> try_read_text_from_file(const String& path);

/// If it exists, load a binary file into a vector of bytes.
SQEE_API std::optional<std::vector<std::byte>> try_read_bytes_from_file(const String& path);

/// Save a string to a text file.
SQEE_API void write_text_to_file(const String& path, StringView text);

/// Save some bytes to a binary file.
SQEE_API void write_bytes_to_file(const String& path, const void* bytes, size_t size);

//============================================================================//

struct SQEE_API TokenisedFile : private MoveOnly
{
    /// Load a text file and tokenise it.
    static TokenisedFile from_file(const String& path);

    /// Take ownership of a string and tokenise it.
    static TokenisedFile from_string(String&& text);

    //--------------------------------------------------------//

    using Tokens = std::vector<StringView>;
    struct Line { Tokens tokens; size_t num; };

    String source;
    std::vector<Line> lines;
};

//============================================================================//

} // namespace sq
