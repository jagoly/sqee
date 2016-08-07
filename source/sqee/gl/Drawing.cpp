#include <sqee/gl/Drawing.hpp>

using namespace sq;

void sq::draw_nothing() {
    static GLuint vao;

    static bool first = true;
    if (first) { first = false;
        gl::CreateVertexArrays(1, &vao);
    }

    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::POINTS, 0, 1);
}

void sq::draw_screen_quad() {
    static GLuint vao;

    static bool first = true;
    if (first) { first = false;
        gl::CreateVertexArrays(1, &vao);
    }

    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
}
