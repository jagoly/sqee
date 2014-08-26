#pragma once

#include <vector>

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

class Framebuffer {
public:
    Framebuffer();
    bool create(int _bufCount, const GLenum* _drawBuffers, bool _depth);
    bool resize(glm::uvec2 _size);
    void use();

    GLuint framebuffer;
    std::vector<Texture2D*> colourTextures;
    TextureDepth depthTexture;
    bool hasDepth;
private:
    std::vector<Texture2D> cTexVec;
    std::vector<GLenum> drawBuffers;
};

}
