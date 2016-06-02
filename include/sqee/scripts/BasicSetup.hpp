#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace chaiscript { class ChaiScript; }
namespace sq { using ChaiEngine = chai::ChaiScript; }

namespace sq {

void chaiscript_setup_app(ChaiEngine& _engine);
void chaiscript_setup_physics(ChaiEngine& _engine);
void chaiscript_setup_maths(ChaiEngine& _engine);

}
