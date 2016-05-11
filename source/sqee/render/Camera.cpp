#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Volumes.hpp>
#include <sqee/render/Camera.hpp>

using namespace sq;

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

void Camera::update() {
    viewMat = maths::look_at(pos, pos + dir, Vec3F(0.f, 0.f, 1.f));
    projMat = maths::perspective(fov, size.x / size.y, rmin, rmax);
    frus = make_Frustum(projMat * viewMat, pos, dir, rmin, rmax);

    struct {
        Mat4F proj, view;
        Mat4F invProj, invView, trnView;
        Vec3F pos; float rmin;
        Vec3F dir; float rmax;
    } ubufData;

    ubufData.proj = projMat;
    ubufData.view = viewMat;
    ubufData.invProj = maths::inverse(projMat);
    ubufData.invView = maths::inverse(viewMat);
    ubufData.trnView = maths::transpose(viewMat);
    ubufData.pos = pos; ubufData.rmin = rmin;
    ubufData.dir = dir; ubufData.rmax = rmax;

    ubo.update(0u, 88u, &ubufData);
}
