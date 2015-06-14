#pragma once
#include <sqee/forward.hpp>

#include <sqee/scripts/chaiscript.hpp>

namespace sq {

void cs_setup_maths(chai::ChaiScript& _cs);
void cs_setup_application(chai::ChaiScript& _cs);
void cs_setup_settings(chai::ChaiScript& _cs);
void cs_setup_console(chai::ChaiScript& _cs);
void cs_setup_render(chai::ChaiScript& _cs);

}
