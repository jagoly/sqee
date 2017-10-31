#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/builtins.hpp>

namespace sq {

//============================================================================//

// todo: make this all use std::from_chars once compiler support is better

inline int sv_to_i(string_view sv) { return int(std::stol(string(sv))); }
inline uint sv_to_u(string_view sv) { return uint(std::stoul(string(sv))); }
inline float sv_to_f(string_view sv) { return float(std::stof(string(sv))); }

inline void parse_tokens(int& out, string_view token) { out = sv_to_i(token); }
inline void parse_tokens(uint& out, string_view token) { out = sv_to_u(token); }
inline void parse_tokens(float& out, string_view token) { out = sv_to_f(token); }

inline optional<int> safe_sv_to_i(string_view sv)
{
    try { return int(std::stol(string(sv))); }
    catch (std::invalid_argument) { return std::nullopt; }
}

inline optional<uint> safe_sv_to_u(string_view sv)
{
    try { return uint(std::stoul(string(sv))); }
    catch (std::invalid_argument) { return std::nullopt; }
}

inline optional<float> safe_sv_to_f(string_view sv)
{
    try { return float(std::stof(string(sv))); }
    catch (std::invalid_argument) { return std::nullopt; }
}

//============================================================================//

template <class T>
inline void parse_tokens(Vector2<T>& out, string_view x, string_view y)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); }

template <class T>
inline void parse_tokens(Vector3<T>& out, string_view x, string_view y, string_view z)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); }

template <class T>
inline void parse_tokens(Vector4<T>& out, string_view x, string_view y, string_view z, string_view w)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); parse_tokens(out.w, w); }

template <class T>
inline void parse_tokens(Quaternion<T>& out, string_view x, string_view y, string_view z, string_view w)
{ parse_tokens(out.x, x); parse_tokens(out.y, y); parse_tokens(out.z, z); parse_tokens(out.w, w); }

//============================================================================//

/// Count the number of tokens in a string_view.
inline size_t count_tokens_in_string(string_view sv, char dlm)
{
    size_t result = 0u;
    size_t begin = 0u, end = 0u;

    while (true)
    {
        if ((begin = sv.find_first_not_of(dlm, end)) == string_view::npos) break;
        ++result;
        if ((end = sv.find_first_of(dlm, begin)) == string_view::npos) break;
    }

    return result;
}

//============================================================================//

struct TokenisedString : sq::MoveOnly
{
    string fullString;
    std::vector<string_view> tokens;
};

/// Split a string_view into a vector of tokens.
std::vector<string_view> tokenise_string_view(string_view sv, char dlm);

/// Split a string into a vector of tokens.
TokenisedString tokenise_string(string str, char dlm);

//===========================================================================

} // namespace sq
