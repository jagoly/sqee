#pragma once

#include <sqee/maths/Vectors.hpp>

namespace sqt { namespace world {

struct CameraObject : sq::NonCopyable {

    /// position of the camera
    Vec3F PROP_position = {0.f, 0.f, 4.f};

    /// direction of the camera
    Vec3F PROP_direction = {0.f, 1.f, 0.f};

    /// vertical field of view
    float PROP_fov = 1.f;
};

}}
