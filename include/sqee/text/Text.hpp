#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

enum class TextBasicFlow { Negative, Positive };
enum class TextBasicAlign { Negative, Centre, Positive };

void render_text_basic(const string& _text, Vec2U _view,
                       TextBasicFlow _hFlow, TextBasicFlow _vFlow,
                       TextBasicAlign _hAlign, TextBasicAlign _vAlign,
                       Vec2F _scale, Vec3F _colour, float _alpha, bool _shadow);

}
