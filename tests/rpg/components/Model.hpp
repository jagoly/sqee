#pragma once

#include <sqee/ecs/Component.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class ModelComponent : public sq::Component {
public:
    ModelComponent(sq::Entity&) {}
    static string type() { return "Model"; }

    /// non-uniform scaling factor
    Vec3F PROP_stretch = {1.f, 1.f, 1.f};

    /// list of materials used
    vector<string> PROP_mtrls = {};

    /// name of the mesh used
    string PROP_mesh = "";

    /// is this model drawn normally
    bool PROP_render = true;

    /// does this model cast shadows
    bool PROP_shadow = true;

    /// does this model receive decals
    bool PROP_decals = true;
};

}
