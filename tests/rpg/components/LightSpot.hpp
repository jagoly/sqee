/*#pragma once

#include <sqee/ecs/Component.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class LightSpotComponent : public sq::Component {
public:
    LightSpotComponent(sq::Entity&) {}
    static string type() { return "LightSpot"; }

    /// maximum colour of light
    Vec3F PROP_colour = {1.f, 1.f, 1.f};

    /// edge softness factor
    float PROP_softness = 0.f;

    /// angle of light cone
    float PROP_angle = 0.125f;

    /// shadow map resolution
    uint PROP_resolution = 0u;
};

}
*/
