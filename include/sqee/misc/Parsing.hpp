// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

// todo: make this all use std::from_chars once compiler support is better

inline int sv_to_i(StringView sv) { return int(std::stol(String(sv))); }
inline uint sv_to_u(StringView sv) { return uint(std::stoul(String(sv))); }
inline float sv_to_f(StringView sv) { return float(std::stof(String(sv))); }

inline void parse_tokens(int& out, StringView token) { out = sv_to_i(token); }
inline void parse_tokens(uint& out, StringView token) { out = sv_to_u(token); }
inline void parse_tokens(float& out, StringView token) { out = sv_to_f(token); }

inline Optional<int> safe_sv_to_i(StringView sv)
{
    try { return int(std::stol(String(sv))); }
    catch (std::invalid_argument&) { return std::nullopt; }
}

inline Optional<uint> safe_sv_to_u(StringView sv)
{
    try { return uint(std::stoul(String(sv))); }
    catch (std::invalid_argument&) { return std::nullopt; }
}

inline Optional<float> safe_sv_to_f(StringView sv)
{
    try { return float(std::stof(String(sv))); }
    catch (std::invalid_argument&) { return std::nullopt; }
}

//============================================================================//

template <class T>
inline void parse_tokens(maths::Vector2<T>& out, StringView x, StringView y)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); }

template <class T>
inline void parse_tokens(maths::Vector3<T>& out, StringView x, StringView y, StringView z)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); }

template <class T>
inline void parse_tokens(maths::Vector4<T>& out, StringView x, StringView y, StringView z, StringView w)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); parse_tokens(out.w, w); }

template <class T>
inline void parse_tokens(maths::Quaternion<T>& out, StringView x, StringView y, StringView z, StringView w)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); parse_tokens(out.w, w); }

//============================================================================//

/// Count the number of tokens in a StringView.
inline size_t count_tokens_in_string(StringView sv, char dlm)
{
    size_t result = 0u;
    size_t begin = 0u, end = 0u;

    while (true)
    {
        if ((begin = sv.find_first_not_of(dlm, end)) == StringView::npos) break;
        ++result;
        if ((end = sv.find_first_of(dlm, begin)) == StringView::npos) break;
    }

    return result;
}

//============================================================================//

struct TokenisedString : sq::MoveOnly
{
    String fullString;
    Vector<StringView> tokens;
};

/// Split a StringView into a vector of tokens.
SQEE_API Vector<StringView> tokenise_string_view(StringView sv, char dlm);

/// Split a StringView into a vector of lineNum, line pairs.
SQEE_API Vector<Pair<uint, StringView>> tokenise_string_view_lines(StringView sv);

/// Split a String into a vector of tokens.
SQEE_API TokenisedString tokenise_string(String str, char dlm);

//===========================================================================

template <size_t S, class T>
inline maths::Vector<S, T> from_string(StringView sv)
{
    const auto tokens = tokenise_string_view(sv, ' ');

    maths::Vector<S, T> result;
    if constexpr (S == 2) parse_tokens(result, tokens[0], tokens[1]);
    if constexpr (S == 3) parse_tokens(result, tokens[0], tokens[1], tokens[2]);
    if constexpr (S == 4) parse_tokens(result, tokens[0], tokens[1], tokens[2], tokens[3]);
    return result;
}

//============================================================================//

} // namespace sq
