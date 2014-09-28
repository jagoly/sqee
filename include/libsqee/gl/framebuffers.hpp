#pragma once

#include <vector>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

class Framebuffer {
public:
    Framebuffer();
    bool create(int _bufCount, const GLenum* _drawBuffers, bool _depth);
    bool resize(glm::uvec2 _size, glm::uvec2 _margins);
    void use();
    void useVP(bool _cutMargins);

    Texture::Ptr get(int _id);

    GLuint framebuffer;
    bool hasDepth;
private:
    std::vector<Texture::Ptr> colourTextures;
    Texture::Ptr depthTexture;
    std::vector<GLenum> drawBuffers;
    glm::uvec2 size;
    glm::uvec2 margins;
};

}