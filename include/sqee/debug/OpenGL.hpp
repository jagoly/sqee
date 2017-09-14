#pragma once

#include <sqee/builtins.hpp>
#include <sqee/export.hpp>

namespace sq {

void MSVC_STDCALL debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei length, const GLchar* message, const void* param);

void debug_message(const string& message);

} // namespace sq
