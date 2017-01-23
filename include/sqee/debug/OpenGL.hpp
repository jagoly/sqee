#pragma once

#include <sqee/builtins.hpp>

namespace sq {

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar* message, const void* param);

void debug_message(const string& message);

} // namespace sq
