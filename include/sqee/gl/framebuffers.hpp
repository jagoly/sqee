#pragma once
#include "forward.hpp"

#include <array>
#include <vector>

#include "gl/textures.hpp"

namespace sq {

class Framebuffer : NonCopyable {
public:
    Framebuffer(const std::vector<GLenum>& _drawBuffers,
                const std::vector<GLenum>& _cFormats,
                const std::vector<GLenum>& _icFormats);
    Framebuffer(GLenum _dsFormat, GLenum _idsFormat);
    Framebuffer(const std::vector<GLenum>& _drawBuffers,
                const std::vector<GLenum>& _cFormats,
                const std::vector<GLenum>& _icFormats,
                GLenum _dsFormat, GLenum _idsFormat);

    void resize(glm::uvec2 _size);
    void resize(uint _size);
    void use();
    void useVP();

    GLuint fbo;

    std::array<Texture, 16> cTexVec;
    Texture dsTex;

private:
    std::vector<GLenum> drawBuffers;
    bool hasDepthStencil = false;
    glm::uvec2 size;
};

}
