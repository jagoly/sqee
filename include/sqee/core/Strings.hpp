#pragma once

#include <sqee/misc/StackString.hpp>

#include <fmt/compile.h>
#include <fmt/format.h>

#include <concepts>
#include <string>
#include <string_view>

namespace sq { //###############################################################

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

constexpr size_t string_length(const char&)
{
    return 1u;
}

constexpr size_t string_length(const std::string_view& sv)
{
    return sv.length();
}

template <std::same_as<char> CharT>
constexpr size_t string_length(const CharT* const& cstr)
{
    return std::char_traits<char>::length(cstr);
}

/// Concatenate a bunch of strings with a single allocation.
template <class... Args>
inline std::string string_concat(const Args&... args)
{
    std::string result;
    result.reserve((string_length(args) + ...));
    ((result += args), ...);
    return result;
}

//==============================================================================

template <StackString fstr, const auto... args>
consteval auto format_consteval()
{
    constexpr auto cfstr = FMT_COMPILE(std::string_view(fstr));
    StackString<fmt::formatted_size(cfstr, args...)> result;
    fmt::format_to(result.data(), cfstr, args...);
    return result;
};

//==============================================================================

/// Only perform formatting if arguments are given.
template <class... Args>
using FmtStrIfArgs = std::conditional_t<sizeof...(Args) != 0, fmt::format_string<Args...>, fmt::string_view>;

/// Version of vformat_to that only does formatting if args is not empty.
template <class OutputIt>
inline OutputIt vformat_to_if_args(OutputIt out, fmt::string_view fstr, fmt::format_args args)
{
    auto&& buf = fmt::detail::get_buffer<char>(out);

    // check if there are any arguments by checking if desc_ has been set
    if (reinterpret_cast<const unsigned long long&>(args) != 0)
        fmt::detail::vformat_to(buf, fstr, args, {});
    else
        buf.append(fstr.data(), fstr.data() + fstr.size());

    return fmt::detail::get_iterator(buf, out);
}

} // namespace sq ##############################################################
