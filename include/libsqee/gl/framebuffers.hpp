#pragma once

#include <vector>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

class Framebuffer {
public:
/// Colour Only
    void create(std::vector<GLenum> _drawBuffers,
                std::vector<GLenum> _cFormats, std::vector<GLenum> _iCFormats);
    void create(GLenum _dsFormat, GLenum _idsFormat);
    void create(std::vector<GLenum> _drawBuffers,
                std::vector<GLenum> _cFormats, std::vector<GLenum> _icFormats,
                GLenum _dsFormat, GLenum _idsFormat);

    void resize(glm::uvec2 _size);
    void clear();
    void clear(glm::vec4 _colour);
    void use();
    void useVP();

    GLuint fbo;

    std::vector<Texture> cTexVec;
    Texture dsTex;
private:
    std::vector<GLenum> drawBuffers;
    glm::uvec2 size;
};

}
