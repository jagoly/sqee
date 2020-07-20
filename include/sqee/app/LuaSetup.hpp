#pragma once

#include <sqee/redist/sol.hpp>
#include <sqee/export.hpp>

namespace sq {

//============================================================================//

SQEE_API void lua_setup_physics(sol::state& lua);
SQEE_API void lua_setup_maths(sol::state& lua);

//============================================================================//

} // namespace sq
