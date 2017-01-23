#include "Camera.hpp"

#include "../../Options.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

CameraData::CameraData() {
    ubo.reserve("view", 16);
    ubo.reserve("proj", 16);
    ubo.reserve("invView", 16);
    ubo.reserve("invProj", 16);
    ubo.reserve("trnView", 16);
    ubo.reserve("pos", 4);
    ubo.reserve("dir", 4);
    ubo.create_and_allocate();
}

void CameraData::refresh(const SceneData& _scene) {

    static const Options& options = Options::get();
    const float rmin = 0.1f, rmax = options.View_Distance;
    const Vec2F size = Vec2F(options.Window_Size);

    const world::CameraObject& camera = *_scene.camera;

    position = camera.PROP_position;
    direction = camera.PROP_direction;

    viewMat = maths::look_at(position, position + direction, {0.f, 0.f, 1.f});
    projMat = maths::perspective(camera.PROP_fov, size.x / size.y, rmin, rmax);

    frustum = maths::make_frustum(projMat * viewMat, position, direction, rmax);

    struct {
        Mat4F view, proj, invView, invProj, trnView;
        Vec3F pos; float a=0; Vec3F dir; float b=0;
    } uboData;

    uboData.proj = projMat;
    uboData.view = viewMat;
    uboData.invProj = maths::inverse(projMat);
    uboData.invView = maths::inverse(viewMat);
    uboData.trnView = maths::transpose(viewMat);
    uboData.pos = position;
    uboData.dir = direction;

    ubo.update(0u, 88u, &uboData);
}
