#pragma once
#include <sqee/forward.hpp>

#include <sqee/redist/gl_ext_4_1.hpp>

namespace sq {

void debug_callback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                    GLsizei _length, const GLchar* _message, const void* _param);

void debug_message(const string& _message);

void debug_draw_texture2D();

void debug_draw_textureCube(uint _face);

}
