#pragma once

#include <sqee/ecs/Component.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class LightOrthoComponent : public sq::Component {
public:
    LightOrthoComponent(sq::Entity&) {}
    static string type() { return "LightOrtho"; }

    /// maximum colour of light
    Vec3F PROP_colour = {1.f, 1.f, 1.f};

    /// left, bottom and near of ortho
    Vec3F PROP_minimum = {-0.5f, -0.5f, 0.f};

    /// right, top and far of ortho
    Vec3F PROP_maximum = {0.5f, 0.5f, 100.f};

    /// light shafts density
    float PROP_density = 0.f;

    /// shadow map resolution
    uint PROP_resolution = 0u;
};

}
