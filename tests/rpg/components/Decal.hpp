#pragma once

#include <sqee/ecs/Component.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class DecalComponent : public sq::Component {
public:
    DecalComponent(sq::Entity&) {}
    static string type() { return "Decal"; }

    /// non-uniform scaling factor
    Vec3F PROP_stretch = {1.f, 1.f, 1.f};

    /// alpha value for fading
    float PROP_alpha = 1.f;

    /// name of the material used
    string PROP_material = "";
};

}
