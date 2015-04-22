#pragma once
#include <sqee/forward.hpp>

#include <sqee/scripts/chaiscript.hpp>

namespace sq {

void cs_setup_glm(chai::ChaiScript& _cs);
void cs_setup_application(chai::ChaiScript& _cs);
void cs_setup_settings(chai::ChaiScript& _cs);
void cs_setup_scene(chai::ChaiScript& _cs);
void cs_setup_handler(chai::ChaiScript& _cs);
void cs_setup_mesh(chai::ChaiScript& _cs);
void cs_setup_skeleton(chai::ChaiScript& _cs);
void cs_setup_skin(chai::ChaiScript& _cs);
void cs_setup_soundmanager(chai::ChaiScript& _cs);
void cs_setup_console(chai::ChaiScript& _cs);

}
