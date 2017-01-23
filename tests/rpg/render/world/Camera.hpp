#pragma once

#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include "../../world/World.hpp"

namespace sqt { namespace render {

struct CameraData : sq::NonCopyable {

    CameraData();

    void refresh(const SceneData& _scene);

    sq::UniformBuffer ubo;
    Vec3F position, direction;
    Mat4F projMat, viewMat;

    sq::maths::Frustum frustum;

//    sys::CullSphere sphere;
//    sys::CullFrustum frustum;
};

}}
