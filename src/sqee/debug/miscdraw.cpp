#include "gl/gl_ext_3_3.hpp"
#include "debug/miscdraw.hpp"

#define DG4X4_TEMPLATE(type, TOP, BOTTOM, LEFT, RIGHT) \
template<> void draw_grid4x4_quad<Grid4x4::type>() { \
    static GLuint vao; \
    static bool first = true; \
    if (first) { first = false; \
        float points[24] { \
            LEFT, BOTTOM, 0.f, 0.f, \
            RIGHT, BOTTOM, 1.f, 0.f, \
            RIGHT, TOP, 1.f, 1.f, \
            RIGHT, TOP, 1.f, 1.f, \
            LEFT, TOP, 0.f, 1.f, \
            LEFT, BOTTOM, 0.f, 0.f, \
        }; \
        GLuint vbo; \
        gl::GenBuffers(1, &vbo); \
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo); \
        gl::BufferData(gl::ARRAY_BUFFER, 24 * sizeof(float), points, gl::STATIC_DRAW); \
        gl::GenVertexArrays(1, &vao); \
        gl::BindVertexArray(vao); \
        gl::EnableVertexAttribArray(0); \
        gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 0, nullptr); \
    } \
    gl::BindVertexArray(vao); \
    gl::DrawArrays(gl::TRIANGLES, 0, 6); \
}

namespace sq {

DG4X4_TEMPLATE(AA, 0.96f, 0.51f, -0.96f, -0.51f)
DG4X4_TEMPLATE(AB, 0.96f, 0.51f, -0.47f, -0.02f)
DG4X4_TEMPLATE(AC, 0.96f, 0.51f, 0.02f, 0.47f)
DG4X4_TEMPLATE(AD, 0.96f, 0.51f, 0.51f, 0.96f)

DG4X4_TEMPLATE(BA, 0.47f, 0.02f, -0.96f, -0.51f)
DG4X4_TEMPLATE(BB, 0.47f, 0.02f, -0.47f, -0.02f)
DG4X4_TEMPLATE(BC, 0.47f, 0.02f, 0.02f, 0.47f)
DG4X4_TEMPLATE(BD, 0.47f, 0.02f, 0.51f, 0.96f)

DG4X4_TEMPLATE(CA, -0.02f, -0.47f, -0.96f, -0.51f)
DG4X4_TEMPLATE(CB, -0.02f, -0.47f, -0.47f, -0.02f)
DG4X4_TEMPLATE(CC, -0.02f, -0.47f, 0.02f, 0.47f)
DG4X4_TEMPLATE(CD, -0.02f, -0.47f, 0.51f, 0.96f)

DG4X4_TEMPLATE(DA, -0.51f, -0.96f, -0.96f, -0.51f)
DG4X4_TEMPLATE(DB, -0.51f, -0.96f, -0.47f, -0.02f)
DG4X4_TEMPLATE(DC, -0.51f, -0.96f, 0.02f, 0.47f)
DG4X4_TEMPLATE(DD, -0.51f, -0.96f, 0.51f, 0.96f)

}

#undef DG4X4_TEMPLATE
