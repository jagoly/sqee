// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/StackString.hpp>

#include <fmt/core.h>

#include <cstring>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace sq {

//============================================================================//

inline const char* to_c_string(const char* arg)
{
    return arg;
}

inline const char* to_c_string(const std::string& arg)
{
    return arg.c_str();
}

template <size_t Capacity>
inline const char* to_c_string(const StackString<Capacity>& arg)
{
    return arg.c_str();
}

//============================================================================//

template <size_t Size>
constexpr size_t string_length(const char(&)[Size])
{
    return Size - 1u;
}

template <size_t Capacity>
constexpr size_t string_length(const StackString<Capacity>& ss)
{
    return ss.length();
}

constexpr size_t string_length(char)
{
    return 1u;
}

constexpr size_t string_length(const std::string_view& sv)
{
    return sv.length();
}

template <class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>>
inline size_t string_length(const CharT* const& cstr)
{
    return std::strlen(cstr);
}

//============================================================================//

/// Function to join a bunch of strings with a single allocation.
template <class... Args>
inline std::string build_string(Args&&... args)
{
    std::string result;
    result.reserve((string_length(std::forward<Args>(args)) + ...));
    ((result += args), ...);
    return result;
}

//============================================================================//

/// Slightly more ergonomic wrapper around fmt::format_to.
template <class... Args>
inline std::string& format_append(std::string& output, std::string_view str, const Args&... args)
{
    fmt::format_to(std::back_inserter(output), str, args...);
    return output;
}

//============================================================================//

} // namespace sq
