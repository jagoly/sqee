#pragma once

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

class Framebuffer {
public:
    void create(const vector<GLenum>& _drawBuffers,
                const vector<GLenum>& _cFormats,
                const vector<GLenum>& _icFormats);
    void create(GLenum _dsFormat, GLenum _idsFormat);
    void create(const vector<GLenum>& _drawBuffers,
                const vector<GLenum>& _cFormats,
                const vector<GLenum>& _icFormats,
                GLenum _dsFormat, GLenum _idsFormat);

    void resize(glm::uvec2 _size);
    void resize(uint _size);
    void use();
    void useVP();

    GLuint fbo;

    array<Texture, 16> cTexVec;
    Texture dsTex;

private:
    vector<GLenum> drawBuffers;
    bool hasDepthStencil = false;
    glm::uvec2 size;
};

}
