#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include "Helpers.hpp"

namespace sqt {

class TransformComponent : public ComponentRPG
                         , public ecs::TagRefresh {
public:
    TransformComponent(EntityRPG&, World&) {}
    static string type() { return "Transform"; }

    // User Properties /////
    Vec3F PROP_position = {0.f, 0.f, 0.f};
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    float PROP_scale    = 1.f;

    // System Properties /////
    Mat4F matrix;
};

}
