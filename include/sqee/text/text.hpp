#pragma once

#include <gl/gl.hpp>
#include <text/font.hpp>

namespace sq {

enum class Alignment { TL, TC, TR, CL, CC, CR, BL, BC, BR };

void draw_tiny_text(const string& _text, float _scale, Alignment _align,
                    glm::vec2 _pos,  glm::uvec2 _viewport);
}
