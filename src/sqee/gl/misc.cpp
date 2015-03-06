#include "gl/misc.hpp"

using namespace sq;

void sq::draw_screen_quad() {
    static GLuint vao;

    static bool first = true;
    if (first) { first = false;
        float points[24] {
            -1.f, -1.f, 0.f, 0.f,
            1.f, -1.f, 1.f, 0.f,
            1.f, 1.f, 1.f, 1.f,
            1.f, 1.f, 1.f, 1.f,
            -1.f, 1.f, 0.f, 1.f,
            -1.f, -1.f, 0.f, 0.f
        };
        GLuint vbo;
        gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, 24 * sizeof(float), points, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 0, nullptr);
    }

    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}
