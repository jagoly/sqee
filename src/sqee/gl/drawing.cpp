#include <sqee/gl/drawing.hpp>

using namespace sq;

extern "C" const float data_ConeVertices[18*3];
extern "C" const uchar data_ConeIndices[32*3];
extern "C" const float data_SphereVertices[42*3];
extern "C" const uchar data_SphereIndices[80*3];
extern "C" const float data_CubeVertices[8*3];
extern "C" const uchar data_CubeIndices[12*3];
const float ScreenQuadVerts[6*2] {
    -1.f, -1.f,    1.f, -1.f,    1.f,  1.f,
     1.f,  1.f,   -1.f,  1.f,   -1.f, -1.f,
};

void sq::draw_screen_quad() {
    static GLuint vao;

    static bool first = true;
    if (first) { first = false;
        GLuint vbo; gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, sizeof(float[6*2]),
                       ScreenQuadVerts, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, nullptr);
    }

    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}


void sq::draw_volume_cone() {
    static GLuint vao, ibo;

    static bool first = true;
    if (first) { first = false;
        GLuint vbo; gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, sizeof(float[18*3]),
                       data_ConeVertices, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

        gl::GenBuffers(1, &ibo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(uchar[32*3]),
                       data_ConeIndices, gl::STATIC_DRAW);
    }

    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::DrawElements(gl::TRIANGLES, 32*3, gl::UNSIGNED_BYTE, 0);
}


void sq::draw_volume_sphr() {
    static GLuint vao, ibo;

    static bool first = true;
    if (first) { first = false;
        GLuint vbo; gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, sizeof(float[42*3]),
                       data_SphereVertices, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

        gl::GenBuffers(1, &ibo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(uchar[80*3]),
                       data_SphereIndices, gl::STATIC_DRAW);
    }

    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::DrawElements(gl::TRIANGLES, 80*3, gl::UNSIGNED_BYTE, 0);
}


void sq::draw_volume_cube() {
    static GLuint vao, ibo;

    static bool first = true;
    if (first) { first = false;
        GLuint vbo; gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, sizeof(float[8*3]),
                       data_CubeVertices, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

        gl::GenBuffers(1, &ibo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(uchar[12*3]),
                       data_CubeIndices, gl::STATIC_DRAW);
    }

    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::DrawElements(gl::TRIANGLES, 12*3, gl::UNSIGNED_BYTE, 0);
}
