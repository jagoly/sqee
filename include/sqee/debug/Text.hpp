// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/misc/Builtins.hpp>
#include <sqee/maths/Builtins.hpp>

namespace sq {

//============================================================================//

SQEE_API void render_text_basic(const String& text, Vec2I flow, Vec2I align, Vec2F scale, Vec4F colour, bool shadow);

//============================================================================//

} // namespace sq
