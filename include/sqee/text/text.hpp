#pragma once

#include <gl/gl.hpp>
#include <text/font.hpp>

namespace sq {

enum class Alignment { TL, TC, TR, CL, CC, CR, BL, BC, BR };

struct TextHandles {
    TextHandles();
    GLuint prog;
    GLuint tex;
    GLuint vbo;
    GLint u_tex;
    GLint u_colour;
};

void draw_tiny_text(const string& _text, float _scale, Alignment _align,
                    glm::vec2 _pos,  glm::uvec2 _viewport);

void draw_text(TextHandles& _handles, Font::Ptr& _font, const string& _text, glm::vec2 _pos, glm::vec2 _sca);

}
