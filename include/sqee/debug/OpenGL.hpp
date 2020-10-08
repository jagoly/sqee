// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/export.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/gl/Types.hpp>

namespace sq {

void __stdcall debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void*);

SQEE_API void debug_message(StringView message);

} // namespace sq
