#include <fstream>

#include <gl/gl.hpp>
#include <misc/files.hpp>

namespace sq {

glm::vec3 get_tangent(glm::vec3 normal) {
    glm::vec3 tangent;
    glm::vec3 t1 = glm::cross(normal, glm::vec3(0, 1, 0));
    glm::vec3 t2 = glm::cross(normal, glm::vec3(0, 0, 1));

    if (glm::length(t1) > glm::length(t2)) {
        tangent = t1;
    } else {
        tangent = t2;
    }
    tangent = glm::normalize(tangent);

    return tangent;
}

void Camera::init(glm::vec3 _pos, float _xRot, float _yRot,
               float _width, float _height, float _yFov, float _zNear, float _zFar) {
    pos.x = _pos.x;
    pos.y = _pos.y;
    pos.z = _pos.z;

    xRot = _xRot;
    yRot = _yRot;

    width = _width;
    height = _height;
    yFov = _yFov;
    zNear = _zNear;
    zFar = _zFar;
}

void Camera::update_viewMat() {
    glm::mat4 rotMat;
    rotMat = glm::rotate(rotMat, xRot, glm::vec3(1.f, 0.f, 0.f));
    rotMat = glm::rotate(rotMat, yRot, glm::vec3(0.f, 1.f, 0.f));

    glm::vec4 forward(0.f, 0.f, -1.f, 0.f);
    forward = rotMat * forward;
    glm::vec3 target = pos + glm::vec3(forward.x, forward.y, forward.z);

    viewMat = glm::lookAt(pos, target, glm::vec3(rotMat * glm::vec4(0.f, 1.f, 0.f, 0.f)));
}

void Camera::update_projMat() {
    projMat = glm::perspective(yFov, width / height, zNear, zFar);
    projAB.x = (zFar + zNear) / (zFar - zNear);
    projAB.y = (2.f * zFar * zNear) / (zFar - zNear);
}

void Camera::update_projViewMat() {
    projViewMat = projMat * viewMat;
}

ScreenQuad::ScreenQuad() {
    GLfloat points[] = {
        -1.0, -1.0,  1.0,  -1.0,  1.0, 1.0,
        1.0, 1.0,    -1.0, 1.0,   -1.0, -1.0
    };
    GLfloat texcoords[] = {
        0.0, 0.0,  1.0, 0.0,  1.0, 1.0,
        1.0, 1.0,  0.0, 1.0,  0.0, 0.0
    };

    GLuint vboPoints, vboTexcoords;

    gl::GenBuffers(1, &vboPoints);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), points, gl::STATIC_DRAW);

    gl::GenBuffers(1, &vboTexcoords);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
    gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), texcoords, gl::STATIC_DRAW);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
    gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, nullptr);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
    gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, nullptr);

    gl::BindVertexArray(0);
}

void ScreenQuad::draw() {
    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);

    gl::BindVertexArray(0);
}

}
