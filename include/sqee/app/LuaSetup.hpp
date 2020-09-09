// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee/

// currently nonfunctional junk header, should be part of rpg

#pragma once

#include <sqee/setup.hpp>

#include <sqee/app/WrenPlus.hpp>

namespace sq {

//============================================================================//

SQEE_API void wren_setup_physics(wren::WrenPlusVM& vm);
SQEE_API void wren_setup_maths(wren::WrenPlusVM& vm);

//============================================================================//

} // namespace sq
