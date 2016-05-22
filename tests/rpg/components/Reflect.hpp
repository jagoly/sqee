#pragma once

#include <sqee/maths/Vectors.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/ecs/Component.hpp>

namespace sqt {

class ReflectComponent : public sq::Component
                       , public sq::ecs::TagRefresh {
public:
    ReflectComponent();

    static string type() { return "Reflect"; }

    // User Properties /////
    float PROP_factor = 1.f;

    // System Properties /////
    sq::UniformBuffer ubo;
    Vec3F normal, trans;
};

}
