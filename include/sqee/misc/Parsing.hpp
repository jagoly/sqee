// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

#include <charconv>
#include <system_error>

namespace sq {

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
