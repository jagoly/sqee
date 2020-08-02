// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee/

#pragma once

#include <sqee/setup.hpp>

#include <sqee/redist/sol.hpp> // IWYU pragma: export

namespace sq {

//============================================================================//

SQEE_API void lua_setup_physics(sol::state& lua);
SQEE_API void lua_setup_maths(sol::state& lua);

//============================================================================//

} // namespace sq
