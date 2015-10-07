#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/render/Camera.hpp>

using namespace sq;

//Camera::~Camera() = default;

Camera::Camera() {
    ubo.reset(new UniformBuffer());
    ubo->reserve("proj", 16);
    ubo->reserve("view", 16);
    ubo->reserve("invProj", 16);
    ubo->reserve("invView", 16);
    ubo->reserve("trnView", 16);
    ubo->reserve("pos", 3);
    ubo->reserve("rmin", 1);
    ubo->reserve("dir", 3);
    ubo->reserve("rmax", 1);
    ubo->allocate_storage();
}

void Camera::update() {
    viewMat = glm::lookAt(pos, pos + dir, {0.f, 0.f, 1.f});
    projMat = glm::perspective(fov, size.x / size.y, rmin, rmax);
    frus = make_Frustum(projMat * viewMat, pos, dir, rmin, rmax);

    struct {
        fmat4 proj, view;
        fmat4 invProj, invView, trnView;
        fvec3 pos; float rmin;
        fvec3 dir; float rmax;
    } ubufData;

    ubufData.proj = projMat;
    ubufData.view = viewMat;
    ubufData.invProj = glm::inverse(projMat);
    ubufData.invView = glm::inverse(viewMat);
    ubufData.trnView = glm::transpose(viewMat);
    ubufData.pos = pos; ubufData.rmin = rmin;
    ubufData.dir = dir; ubufData.rmax = rmax;

    ubo->update(0u, 88u, &ubufData);
}
