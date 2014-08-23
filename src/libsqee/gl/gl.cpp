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

Camera::Camera(glm::vec3 _pos, float _xRot, float _yRot,
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

Camera::Camera() {

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

}
