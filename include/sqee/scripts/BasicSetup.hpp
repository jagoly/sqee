#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace chaiscript { class ChaiScript; }

namespace sq {

void cs_setup_app(chai::ChaiScript& _cs);
void cs_setup_render(chai::ChaiScript& _cs);
void cs_setup_maths(chai::ChaiScript& _cs);

}
