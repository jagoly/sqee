#pragma once

#include <sqee/setup.hpp>

namespace chaiscript { class ChaiScript; }
namespace sqt { class World; }

namespace sqt {

void cs_setup_world(chai::ChaiScript& _cs);
void cs_setup_renderer(chai::ChaiScript& _cs);

void cs_setup_components(chai::ChaiScript& _cs, World& _world);
void cs_setup_functions(chai::ChaiScript& _cs, World& _world);

}
