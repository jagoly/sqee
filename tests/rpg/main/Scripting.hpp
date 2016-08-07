#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace chaiscript { class ChaiScript; }
namespace sq { using ChaiEngine = chai::ChaiScript; }

namespace sqt {

void chaiscript_setup_world(sq::ChaiEngine& _engine);
void chaiscript_setup_components(sq::ChaiEngine& _engine);
void chaiscript_setup_functions(sq::ChaiEngine& _engine);
void chaiscript_setup_messages(sq::ChaiEngine& _engine);

}
