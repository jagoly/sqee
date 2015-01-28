#pragma once
#include <defs.hpp>

#include <gl/gl_ext_3_3.hpp>

namespace sq {

void APIENTRY debug_callback(GLenum _source, GLenum _type, GLuint /*_id*/, GLenum _severity,
                   GLsizei /*_length*/, const GLchar* _message, const void* /*_param*/);

void draw_screen_quad();

}
