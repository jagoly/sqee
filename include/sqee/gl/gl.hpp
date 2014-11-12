#pragma once
#include <defs.hpp>

#include <gl/gl_ext_3_3.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sq {

const glm::mat4 iMat4;

void debug_callback(GLenum _source, GLenum _type, GLuint /*_id*/, GLenum _severity,
                   GLsizei /*_length*/, const GLchar* _message, const void* /*_param*/);

struct ScreenQuad {
    ScreenQuad();
    void draw();
    GLuint vao;
};

}
