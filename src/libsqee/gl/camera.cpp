#include <iostream>

#include <gl/camera.hpp>

using namespace sq;

Camera::Camera() {
    gl::GenBuffers(1, &ubo);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo);
    gl::BufferData(gl::UNIFORM_BUFFER, 44 * sizeof(GLfloat), nullptr, gl::DYNAMIC_DRAW);
}

void Camera::init(glm::vec3 _pos, glm::vec3 _rot, float _width, float _height,
                  float _yFov, float _zNear, float _zFar) {
    pos = _pos;
    rot = _rot;

    width = _width;
    height = _height;
    yFov = _yFov;
    zRange = {_zNear, _zFar};
}

void Camera::update_viewMat() {
    glm::mat4 rotMat;
    if (rot.x) rotMat = glm::rotate(rotMat, rot.x, glm::vec3(1, 0, 0));
    if (rot.y) rotMat = glm::rotate(rotMat, rot.y, glm::vec3(0, 1, 0));
    if (rot.z) rotMat = glm::rotate(rotMat, rot.z, glm::vec3(0, 0, 1));

    glm::vec4 forward(0.f, 0.f, -1.f, 0.f);
    forward = rotMat * forward;
    glm::vec3 target = pos + glm::vec3(forward.x, forward.y, forward.z);

    viewMat = glm::lookAt(pos, target, glm::vec3(rotMat * glm::vec4(0.f, 1.f, 0.f, 0.f)));
}

void Camera::update_projMat() {
    projMat = glm::perspective(yFov, width / height, zRange.x, zRange.y);
}

void Camera::update_ubo() {
    gl::BindBufferBase(gl::UNIFORM_BUFFER, 0, ubo);
    gl::BufferSubData(gl::UNIFORM_BUFFER, 0, 16*sizeof(GLfloat),
                      glm::value_ptr(projMat));
    gl::BufferSubData(gl::UNIFORM_BUFFER, 16*sizeof(GLfloat), 16*sizeof(GLfloat),
                      glm::value_ptr(viewMat));
    gl::BufferSubData(gl::UNIFORM_BUFFER, 32*sizeof(GLfloat), 3*sizeof(GLfloat),
                      glm::value_ptr(pos));
    gl::BufferSubData(gl::UNIFORM_BUFFER, 36*sizeof(GLfloat), 3*sizeof(GLfloat),
                      glm::value_ptr(rot));
    gl::BufferSubData(gl::UNIFORM_BUFFER, 40*sizeof(GLfloat), 2*sizeof(GLfloat),
                      glm::value_ptr(zRange));
}
