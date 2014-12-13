#include <gl/cameras.hpp>

using namespace sq;

BaseCamera::BaseCamera(bool _useUBO) : useUBO(_useUBO) {
    if (useUBO) {
        ubo.reserve("proj", 16);
        ubo.reserve("view", 16);
        ubo.reserve("pos", 3);
        ubo.reserve("near", 1);
        ubo.reserve("dir", 3);
        ubo.reserve("far", 1);
        ubo.create();
    }
}

void BaseCamera::update() {
    if (useUBO) {
        ubo.bind(0);
        ubo.update("proj", &projMat);
        ubo.update("view", &viewMat);
        ubo.update("pos", &pos);
        ubo.update("near", &range.x);
        ubo.update("dir", &dir);
        ubo.update("far", &range.y);
    }
}


void LookatCamera::update() {
    viewMat = glm::lookAt(pos, pos+dir, {0, 1, 0});

    projMat = glm::perspective(fov, size.x / size.y, range.x, range.y);
    if (strictAspect > 0.f) {
        if (size.x > size.y * strictAspect) {
            float tanFov = std::tan(0.5f * fov);
            projMat[0][0] = 1.f / tanFov / strictAspect;
            projMat[1][1] = 1.f / (size.y / size.x * tanFov) / strictAspect;
        }
    }

    BaseCamera::update();
}
