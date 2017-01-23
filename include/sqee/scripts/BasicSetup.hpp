#pragma once

#include <sqee/scripts/ChaiEngine.hpp>

namespace sq {

//============================================================================//

void chaiscript_setup_app(ChaiEngine& engine);
void chaiscript_setup_physics(ChaiEngine& engine);
void chaiscript_setup_maths(ChaiEngine& engine);
void chaiscript_setup_messages(ChaiEngine& engine);

//============================================================================//

} // namespace sq
