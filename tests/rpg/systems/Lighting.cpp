#include <sqee/dop/Functions.hpp>

#include "Transform.hpp"
#include "Lighting.hpp"

namespace dop = sq::dop;
using namespace sqt::sys;

//============================================================================//

void SpotLightEdit::set_colour(Vec3F colour) { mEntry->colour = colour; }

void SpotLightEdit::set_softness(float softness) { mEntry->softness = softness; }

void SpotLightEdit::set_angle(float angle) { mEntry->angle = angle; }

void SpotLightEdit::set_resolution(uint resolution) { mEntry->resolution = resolution; }

//============================================================================//

LightingSystem::LightingSystem() : mStuff(static_WorldStuff()) {}

//============================================================================//

void LightingSystem::system_refresh()
{
    auto& tables = mStuff.tables;

    auto joinResult = dop::joined(sq::as_const(tables.transform), tables.spotLight);

    for (auto& joinEntry : joinResult)
    {
        const auto& transform = std::get<1>(joinEntry);
        const auto& light = std::get<2>(joinEntry);
    }
}

//============================================================================//

LightingSystem& sqt::sys::static_LightingSystem()
{
    static LightingSystem system;
    return system;
}
