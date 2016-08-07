#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt { namespace world {

struct AmbientObject : sq::NonCopyable {

    /// global ambient light colour
    Vec3F PROP_colour = {1.f, 1.f, 1.f};
};

}}
