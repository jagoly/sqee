#pragma once

#include "WorldStuff.hpp"

namespace sqt { namespace sys {

//============================================================================//

void system_tick_animations(WorldStuff& stuff);

void system_blend_animations(WorldStuff& stuff, float factor);

//============================================================================//

}} // namespace sqt::sys
