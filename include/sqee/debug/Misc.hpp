#pragma once

#include <sqee/setup.hpp>

namespace sq {

void debug_draw_texture2D();

void debug_draw_textureCube(uint _face);

void debug_draw_texture2DArray(uint _layer);

void debug_draw_textureCubeArray(uint _layer, uint _face);

}
