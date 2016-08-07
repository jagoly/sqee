#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sqt { namespace world {

struct SkyLightObject : sq::NonCopyable {

    /// rotation of the light
    QuatF PROP_rotation = {0.f, 0.f, 0.f, 1.f};

    /// maximum colour of light
    Vec3F PROP_colour = {1.f, 1.f, 1.f};

    /// light shafts density
    float PROP_density = 0.f;

    /// shadow map resolution
    uint PROP_resolution = 1024u;

    /// number of cascades
    uint PROP_cascades = 4u;
};

}}
