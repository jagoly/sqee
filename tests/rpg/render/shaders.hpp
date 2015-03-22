#pragma once
#include <sqee/forward.hpp>

namespace sqt {

sq::Shader* shader_modl_static();
sq::Shader* shader_modl_skelly();
sq::Shader* shader_shad_static();
sq::Shader* shader_shad_skelly();
sq::Shader* shader_shad_static_punch();
sq::Shader* shader_shad_skelly_punch();
sq::Shader* shader_gnrc_quad();
sq::Shader* shader_gnrc_lines();

sq::Shader* shader_modl_write();
sq::Shader* shader_shds_ambient();
sq::Shader* shader_shds_skylight();
sq::Shader* shader_shds_spotlight();

}
