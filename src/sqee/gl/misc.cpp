#include <iostream>

#include "gl/misc.hpp"

using namespace sq;

void sq::debug_callback(GLenum _source, GLenum _type, GLuint /*_id*/, GLenum _severity,
                        GLsizei /*_length*/, const GLchar* _message, const void* /*_param*/) {
    string source;
    if (_source == gl::DEBUG_SOURCE_API)
        source = "API";
    else if (_source == gl::DEBUG_SOURCE_APPLICATION)
        source = "APPLICATION";
    else if (_source == gl::DEBUG_SOURCE_OTHER)
        source = "OTHER";
    else if (_source == gl::DEBUG_SOURCE_SHADER_COMPILER)
        source = "SHADER_COMPILER";
    else if (_source == gl::DEBUG_SOURCE_THIRD_PARTY)
        source = "THIRD_PARTY";
    else if (_source == gl::DEBUG_SOURCE_WINDOW_SYSTEM)
        source = "WINDOW_SYSTEM";

    string type;
    if (_type == gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        type = "DEPRECATED_BEHAVIOR";
    if (_type == gl::DEBUG_TYPE_ERROR)
        type = "ERROR";
    if (_type == gl::DEBUG_TYPE_MARKER)
        type = "MARKER";
    if (_type == gl::DEBUG_TYPE_OTHER)
        type = "OTHER";
    if (_type == gl::DEBUG_TYPE_PERFORMANCE)
        type = "PERFORMANCE";
    if (_type == gl::DEBUG_TYPE_POP_GROUP)
        type = "POP_GROUP";
    if (_type == gl::DEBUG_TYPE_PORTABILITY)
        type = "PORTABILITY";
    if (_type == gl::DEBUG_TYPE_PUSH_GROUP)
        type = "PUSH_GROUP";
    if (_type == gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        type = "UNDEFINED_BEHAVIOR";

    string severity;
    if (_severity == gl::DEBUG_SEVERITY_HIGH)
        severity = "HIGH";
    if (_severity == gl::DEBUG_SEVERITY_LOW)
        severity = "LOW";
    if (_severity == gl::DEBUG_SEVERITY_MEDIUM)
        severity = "MEDIUM";
    if (_severity == gl::DEBUG_SEVERITY_NOTIFICATION)
        severity = "NOTIFICATION";

    std::cout << "\nSource: " << source
              << " | Severity: " << severity
              << " | Type: " << type
              << "\nMessage: " << _message
              << std::endl;
}

void sq::draw_screen_quad() {
    static GLuint vao;

    static bool first = true;
    if (first) { first = false;
        float points[12] = {
            -1.0, -1.0, 1.0, -1.0, 1.0, 1.0,
            1.0, 1.0, -1.0, 1.0, -1.0, -1.0
        };
        GLuint vbo;
        gl::GenBuffers(1, &vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(float), points, gl::STATIC_DRAW);


        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, nullptr);
    }

    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}