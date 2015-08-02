#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/render/Camera.hpp>

using namespace sq;

Camera::~Camera() = default;

Camera::Camera(GLuint _bind) : binding(_bind) {
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
    ubo->reserve("fov", 1);
    ubo->create();
}

void Camera::update() {
    viewMat = glm::lookAt(pos, pos + dir, {0.f, 0.f, 1.f});
    projMat = glm::perspective(fov, size.x / size.y, rmin, rmax);
    frus = make_Frustum(projMat*viewMat, pos, dir, rmin, rmax);

    fmat4 invProj = glm::inverse(projMat);
    fmat4 invView = glm::inverse(viewMat);
    fmat4 trnView = glm::transpose(viewMat);

    float aspect = size.x / size.y;

    ubo->bind(binding);
    ubo->update("proj", &projMat);
    ubo->update("view", &viewMat);
    ubo->update("invProj", &invProj);
    ubo->update("invView", &invView);
    ubo->update("trnView", &trnView);
    ubo->update("pos", &pos);
    ubo->update("rmin", &rmin);
    ubo->update("dir", &dir);
    ubo->update("rmax", &rmax);
    ubo->update("fov", &aspect);
}
