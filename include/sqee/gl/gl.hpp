#pragma once
#include <defs.hpp>

#include <gl/gl_ext_3_3.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace sq {

enum class UType : GLenum {
    f1 = gl::FLOAT, f2 = gl::FLOAT_VEC2,
    f3 = gl::FLOAT_VEC3, f4 = gl::FLOAT_VEC4,
    i1 = gl::INT, i2 = gl::INT_VEC2,
    i3 = gl::INT_VEC3, i4 = gl::INT_VEC4,
    u1 = gl::UNSIGNED_INT, u2 = gl::UNSIGNED_INT_VEC2,
    u3 = gl::UNSIGNED_INT_VEC3, u4 = gl::UNSIGNED_INT_VEC4,
    m2 = gl::FLOAT_MAT2, m23 = gl::FLOAT_MAT2x3, m24 = gl::FLOAT_MAT2x4,
    m3 = gl::FLOAT_MAT3, m32 = gl::FLOAT_MAT3x2, m34 = gl::FLOAT_MAT3x4,
    m4 = gl::FLOAT_MAT4, m42 = gl::FLOAT_MAT4x2, m43 = gl::FLOAT_MAT4x3
};

const glm::mat4 iMat4;

void debug_callback(GLenum _source, GLenum _type, GLuint /*_id*/, GLenum _severity,
                   GLsizei /*_length*/, const GLchar* _message, const void* /*_param*/);

struct ScreenQuad {
    ScreenQuad();
    void draw();
    GLuint vao;
};

}
