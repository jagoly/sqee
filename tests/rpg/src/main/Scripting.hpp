#pragma once

#include <sqee/redist/sol.hpp>

//============================================================================//

namespace sqt {

void lua_setup_world(sol::state& lua);

void lua_setup_api(sol::state& lua);

} // namespace sqt
