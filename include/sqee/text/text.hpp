#pragma once
#include "forward.hpp"

#include <string>

namespace sq {

void draw_tiny_text(const std::string& _text, float _scale, Alignment _align,
                    glm::vec2 _pos,  glm::uvec2 _viewport);
}
