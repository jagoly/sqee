#pragma once
#include <sqee/forward.hpp>

namespace sq {

enum class TextBasicFlow { Negative, Positive };
enum class TextBasicAlign { Negative, Centre, Positive };

void render_text_basic(const string& _text, uvec2 _view,
                       TextBasicFlow _hFlow, TextBasicFlow _vFlow,
                       TextBasicAlign _hAlign, TextBasicAlign _vAlign,
                       fvec2 _scale, fvec3 _colour, float _alpha, bool _shadow);

}
