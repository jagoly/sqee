#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Vectors.hpp>

namespace sq {

//============================================================================//

void render_text_basic(const string& text, Vec2I flow, Vec2I align, Vec2F scale, Vec4F colour, bool shadow);

//============================================================================//

} // namespace sq
