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

} // namespace sq
