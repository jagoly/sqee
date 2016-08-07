#pragma once

#include <sqee/ecs/Component.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class LightPointComponent : public sq::Component {
public:
    LightPointComponent(sq::Entity&) {}
    static string type() { return "LightPoint"; }

    /// maximum colour of light
    Vec3F PROP_colour = {1.f, 1.f, 1.f};

    /// shadow map resolution
    uint PROP_resolution = 0u;
};

}
