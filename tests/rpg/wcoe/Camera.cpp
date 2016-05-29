#include "Camera.hpp"

using namespace sqt;
namespace maths = sq::maths;

Camera::Camera() {
    ubo.reserve("proj", 16);
    ubo.reserve("view", 16);
    ubo.reserve("invProj", 16);
    ubo.reserve("invView", 16);
    ubo.reserve("trnView", 16);
    ubo.reserve("pos", 3);
    ubo.reserve("rmin", 1);
    ubo.reserve("dir", 3);
    ubo.reserve("rmax", 1);
    ubo.create_and_allocate();
}

void Camera::configure() {
}

void Camera::update() {
    projMat = maths::perspective(PROP_fov, size.x / size.y, rmin, rmax);
    viewMat = maths::look_at(PROP_position, PROP_position + PROP_direction, {0.f, 0.f, 1.f});
    frus = make_Frustum(projMat * viewMat, PROP_position, PROP_direction, rmin, rmax);

    struct {
        Mat4F proj, view, invProj, invView, trnView;
        Vec3F pos; float rmin; Vec3F dir; float rmax;
    } uboData;

    uboData.proj = projMat;
    uboData.view = viewMat;
    uboData.invProj = maths::inverse(projMat);
    uboData.invView = maths::inverse(viewMat);
    uboData.trnView = maths::transpose(viewMat);
    uboData.pos = PROP_position; uboData.rmin = rmin;
    uboData.dir = PROP_direction; uboData.rmax = rmax;

    ubo.update(0u, 88u, &uboData);
}
