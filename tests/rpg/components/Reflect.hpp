#pragma once

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>

#include "Helpers.hpp"

namespace sqt {

class ReflectComponent : public ComponentRPG
                       , public ecs::TagRefresh {
public:
    ReflectComponent(EntityRPG&, World&);
    static string type() { return "Reflect"; }

    // User Properties /////
    float PROP_factor = 1.f;

    // System Properties /////
    sq::UniformBuffer ubo;
    Vec3F normal, trans;
};

}
