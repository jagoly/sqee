#pragma once

#include <sqee/setup.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

struct RpgOptions {
    Vec2U WindowSize   = {800u, 600u};
    float ViewDistance = 120.f;
    bool  ShadowFilter = true;
    bool  ShadowLarge  = true;
    bool  Vignette     = true;
    bool  Bloom        = true;
    int   Shafts       = 2;
    int   SSAO         = 2;
    int   FSAA         = 2;
};

}
