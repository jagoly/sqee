// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

void MSVC_STDCALL debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar* message, const void* param);

SQEE_API void debug_message(StringView message);

} // namespace sq
