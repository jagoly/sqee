#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/uniformbuffers.hpp>
#include "sqee/render/cameras.hpp"

using namespace sq;

Camera::Camera() {}

void Camera::update() {
    viewMat = glm::lookAt(pos, pos+dir, {0,0,1});
    projMat = glm::perspective(fov, size.x / size.y, range.x, range.y);
    frus = make_Frustum(glm::inverse(projMat * viewMat));
}


UboCamera::UboCamera(const vector<pair<string, uint>>& _extra) {
    ubo.reset(new Uniformbuffer());
    ubo->reserve("proj", 16);
    ubo->reserve("view", 16);
    ubo->reserve("invProj", 16);
    ubo->reserve("invView", 16);
    ubo->reserve("trnView", 16);
    ubo->reserve("pos", 3);
    ubo->reserve("near", 1);
    ubo->reserve("dir", 3);
    ubo->reserve("far", 1);
    for (const auto& ext : _extra)
        ubo->reserve(ext.first, ext.second);
    ubo->create();
}

void UboCamera::update() {
    Camera::update();

    mat4 invProj = glm::inverse(projMat);
    mat4 invView = glm::inverse(viewMat);
    mat4 trnView = glm::transpose(viewMat);

    ubo->bind(binding);
    ubo->update("proj", &projMat);
    ubo->update("view", &viewMat);
    ubo->update("invProj", &invProj);
    ubo->update("invView", &invView);
    ubo->update("trnView", &trnView);
    ubo->update("pos", &pos);
    ubo->update("near", &range.x);
    ubo->update("dir", &dir);
    ubo->update("far", &range.y);
}
