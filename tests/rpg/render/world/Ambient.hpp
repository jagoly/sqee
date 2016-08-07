#pragma once

#include <sqee/maths/Vectors.hpp>

#include "../../world/objects/Ambient.hpp"

namespace sqt { namespace render {

struct AmbientData {

    // update object from world data
    void refresh(const world::AmbientObject& _obj);

    // data required for rendering
    Vec3F colour;
};

}}
