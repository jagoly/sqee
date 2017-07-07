#pragma once

#include <sqee/app/ChaiEngine.hpp>

namespace sqt {

//============================================================================//

void chaiscript_setup_world(sq::ChaiEngine& engine);

void chaiscript_setup_api(sq::ChaiEngine& engine);

void chaiscript_setup_messages(sq::ChaiEngine& engine);

//============================================================================//

} // namespace sqt
