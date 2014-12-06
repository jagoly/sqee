#include <gl/cameras.hpp>

using namespace sq;

Camera::Camera() {
    ubo.reserve("projMat", 16);
    ubo.reserve("viewMat", 16);
    ubo.reserve("camPos", 3);
    ubo.reserve("zNear", 1);
    ubo.reserve("camDir", 3);
    ubo.reserve("zFar", 1);
    ubo.create();
}

void Camera::init(glm::vec3 _pos, glm::vec3 _dir, glm::vec2 _zRange,
                  glm::vec2 _aspect, float _fov) {
    pos = _pos;
    dir = _dir;
    zRange = _zRange;
    aspect = _aspect;
    fov = _fov;
}

void Camera::update_viewMat() {
    viewMat = glm::lookAt(pos, pos+dir, {0, 1, 0});
}

void Camera::update_projMat(float _aspStnd) {
    projMat = glm::perspective(fov, aspect.x / aspect.y, zRange.x, zRange.y);
    if (_aspStnd != 0.f) {
        if (aspect.x > aspect.y * _aspStnd) {
            float tanFov = std::tan(0.5f * fov);
            projMat[0][0] = 1.f / tanFov / _aspStnd;
            projMat[1][1] = 1.f / (aspect.y / aspect.x * tanFov) / _aspStnd;
        }
    }
}

void Camera::update_ubo() {
    ubo.bind(0);
    ubo.update("projMat", &projMat);
    ubo.update("viewMat", &viewMat);
    ubo.update("camPos", &pos);
    ubo.update("zNear", &zRange.x);
    ubo.update("camDir", &dir);
    ubo.update("zFar", &zRange.y);
}
