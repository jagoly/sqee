// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>

namespace sq {

void MSVC_STDCALL debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar* message, const void* param);

SQEE_API void debug_message(const String& message);

} // namespace sq
