#pragma once

#include "WorldStuff.hpp"

namespace sqt::sys {

//============================================================================//

void system_tick_animations(WorldStuff& stuff);

void system_blend_animations(WorldStuff& stuff, float tickBlend);

//============================================================================//

} // namespace sqt::sys
