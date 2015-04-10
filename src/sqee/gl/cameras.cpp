#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/app/logging.hpp"
#include "sqee/gl/cameras.hpp"

using namespace sq;

Camera::Camera() {}

void Camera::update() {
    viewMat = glm::lookAt(pos, pos+dir, {0,0,1});
    projMat = glm::perspective(fov, size.x / size.y, range.x, range.y);
    frus = make_Frustum(glm::inverse(projMat * viewMat));
}


UboCamera::UboCamera() {
    ubo.reserve("proj", 16);
    ubo.reserve("view", 16);
    ubo.reserve("pos", 3);
    ubo.reserve("near", 1);
    ubo.reserve("dir", 3);
    ubo.reserve("far", 1);
}

void UboCamera::update() {
    Camera::update();

    ubo.bind(binding);
    ubo.update("proj", &projMat);
    ubo.update("view", &viewMat);
    ubo.update("pos", &pos);
    ubo.update("near", &range.x);
    ubo.update("dir", &dir);
    ubo.update("far", &range.y);
}
