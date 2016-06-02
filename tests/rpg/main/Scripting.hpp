#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace chaiscript { class ChaiScript; }
namespace sq { using ChaiEngine = chai::ChaiScript; }
namespace sqt { class World; }

namespace sqt {

void chaiscript_setup_world(sq::ChaiEngine& _engine, World& _world);
void chaiscript_setup_components(sq::ChaiEngine& _engine, World& _world);
void chaiscript_setup_functions(sq::ChaiEngine& _engine, World& _world);

}
