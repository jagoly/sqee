#pragma once
#include <sqee/forward.hpp>

namespace sq {

struct TextBasic {
    enum class Flow { Negative, Positive };
    enum class Align { Negative, Centre, Positive };
    Flow hFlow, vFlow; Align hAlign, vAlign;
    fvec3 colour; fvec2 scale; bool shadow;
};

void render_text_basic(const string& _text, const TextBasic& _tb, float _alpha, uvec2 _view);

}
