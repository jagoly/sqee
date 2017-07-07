#pragma once

#include <sqee/dop/Classes.hpp>
#include <sqee/maths/Volumes.hpp>

#include "WorldStuff.hpp"

namespace sqt::sys {

//============================================================================//

void system_refresh_culling(WorldStuff& stuff);

sq::dop::Group system_get_visible_objects_sphere(const WorldStuff& stuff, sq::maths::Sphere sphere);

sq::dop::Group system_get_visible_objects_frustum(const WorldStuff& stuff, sq::maths::Frustum frustum);

sq::dop::Group system_get_visible_objects_planes(const WorldStuff& stuff, sq::maths::Planes<4> planes);

//============================================================================//

} // namespace sqt::sys
