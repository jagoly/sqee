#pragma once
#include <memory>

#include <freetype2/ft2build.h>

#include <gl/gl.hpp>
#include <text/font.hpp>

namespace sq {

struct TextHandles {
    TextHandles();
    GLuint prog;
    GLuint tex;
    GLuint vbo;
    GLint u_tex;
    GLint u_colour;
};

void draw_text(TextHandles& _handles, Font::Ptr& _font, const std::string& _text, glm::vec2 _pos, glm::vec2 _sca);

}
