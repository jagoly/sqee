#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/ecs/Component.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class TransformComponent : public sq::Component {
public:
    static string type() { return "Transform"; }

    void setup_depends(sq::Entity* _e);

    // User Properties /////
    Vec3F PROP_position = {0.f, 0.f, 0.f};
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};
    float PROP_scale    = 1.f;

    // System Properties /////
    Mat4F matrix;

    // Soft Depended Components /////
    const TransformComponent* PRNT_Transform = nullptr;
};

}
