// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//============================================================================//

SQEE_API void render_text_basic(StringView text, Vec2I flow, Vec2I align, Vec2F scale, Vec4F colour, bool shadow);

//============================================================================//

} // namespace sq
