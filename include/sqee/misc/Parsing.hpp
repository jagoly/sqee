// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <charconv>

namespace sq {

//============================================================================//

/// Split a string into a fixed number of segments. Check for success with `!result.back().empty()`.
template <char... Delimiters>
constexpr std::array<StringView, sizeof...(Delimiters)+1> split_string(StringView sv) noexcept
{
    constexpr char delimiters[] = { Delimiters... };

    std::array<StringView, sizeof...(Delimiters)+1> result;
    for (size_t index = 0u; index < sizeof...(Delimiters) && !sv.empty(); ++index)
    {
        size_t segSize = sv.find(delimiters[index]);
        if (segSize == StringView::npos) segSize = sv.size();
        result[index] = StringView(sv.data(), segSize);
        if (segSize < sv.size()) ++segSize;
        sv.remove_prefix(segSize);
    }
    result.back() = sv;

    return result;
}

//============================================================================//

/// Split a StringView into a vector of tokens.
SQEE_API std::vector<StringView> tokenise_string(StringView sv, char dlm);

/// Split a StringView into a vector of tokens.
SQEE_API void tokenise_string(StringView sv, char dlm, std::vector<StringView>& out);

/// Split a StringView into lines, keeping empty lines.
SQEE_API std::vector<StringView> tokenise_string_lines(StringView sv);

//============================================================================//

template <class Number>
inline Number parse_number(StringView sv)
{
    Number number;
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), number);
    if (result.ec == std::errc::invalid_argument) throw std::invalid_argument("parse_number");
    if (result.ec == std::errc::result_out_of_range) throw std::out_of_range("parse_number");
    return number;
}

template <class Number>
inline void parse_number(Number& out, StringView sv)
{
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), out);
    if (result.ec == std::errc::invalid_argument) throw std::invalid_argument("parse_number");
    if (result.ec == std::errc::result_out_of_range) throw std::out_of_range("parse_number");
}

#ifdef _LIBCPP_VERSION // libc++ still doesn't support floating point from_chars as of dec 2023
template <class Number>
inline Number parse_number(StringView sv) requires std::floating_point<Number>
{
    return Number(std::stod(String(sv)));
}
template <class Number>
inline void parse_number(Number& out, StringView sv) requires std::floating_point<Number>
{
    out = Number(std::stod(String(sv)));
}
#endif

//============================================================================//

template <class T>
inline void parse_numbers(maths::Vector<2, T>& out, StringView x, StringView y)
{
    parse_number(out.x, x);
    parse_number(out.y, y);
}

template <class T>
inline void parse_numbers(maths::Vector<3, T>& out, StringView x, StringView y, StringView z)
{
    parse_number(out.x, x);
    parse_number(out.y, y);
    parse_number(out.z, z);
}

template <class T>
inline void parse_numbers(maths::Vector<4, T>& out, StringView x, StringView y, StringView z, StringView w)
{
    parse_number(out.x, x);
    parse_number(out.y, y);
    parse_number(out.z, z);
    parse_number(out.w, w);
}

template <class T>
inline void parse_numbers(maths::Quaternion<T>& out, StringView x, StringView y, StringView z, StringView w)
{
    parse_number(out.x, x);
    parse_number(out.y, y);
    parse_number(out.z, z);
    parse_number(out.w, w);
}

//============================================================================//

template <class T>
inline void parse_numbers_normalize(maths::Vector<2, T>& out, StringView x, StringView y)
{
    parse_numbers(out, x, y);
    out = maths::normalize(out);
}

template <class T>
inline void parse_numbers_normalize(maths::Vector<3, T>& out, StringView x, StringView y, StringView z)
{
    parse_numbers(out, x, y, z);
    out = maths::normalize(out);
}

template <class T>
inline void parse_numbers_normalize(maths::Vector<4, T>& out, StringView x, StringView y, StringView z, StringView w)
{
    parse_numbers(out, x, y, z, w);
    out = maths::normalize(out);
}

template <class T>
inline void parse_numbers_normalize(maths::Quaternion<T>& out, StringView x, StringView y, StringView z, StringView w)
{
    parse_numbers(out, x, y, z, w);
    out = maths::normalize(out);
}

//============================================================================//

} // namespace sq
