#pragma once

#include <sqee/ecs/Component.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sqt {

class TransformComponent : public sq::Component {
public:
    TransformComponent(sq::Entity&) {}
    static string type() { return "Transform"; }

    /// local-space position
    Vec3F PROP_position = {0.f, 0.f, 0.f};

    /// local-space rotation
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};

    /// local-space scale
    float PROP_scale = 1.f;

    /// calculated matrix
    Mat4F matrix;
};

}
