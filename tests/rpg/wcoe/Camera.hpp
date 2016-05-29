#pragma once

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Volumes.hpp>

namespace sqt {

class Camera final : sq::NonCopyable {
public:
    Camera();

    void configure();
    void update();

    // User Properties /////
    Vec3F PROP_position  = {0.f, 0.f, 4.f};
    Vec3F PROP_direction = {0.f, 1.f, 0.f};
    float PROP_fov       = 1.f;

    // System Properties /////
    sq::UniformBuffer ubo;
    float rmin = 0.1f;
    float rmax = 40.f;
    sq::Frustum frus;
    Mat4F projMat;
    Mat4F viewMat;
    Vec2F size;
};

}
