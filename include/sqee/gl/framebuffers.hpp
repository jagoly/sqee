#pragma once
#include "forward.hpp"

#include <vector>

#include "gl/textures.hpp"

namespace sq {

class Framebuffer : NonCopyable {
public:
    Framebuffer();

    void create_tex(int _id, GLenum _format, GLenum _iFormat, Texture::Preset _preset);

    void resize(glm::uvec2 _size);
    void use();
    void useVP();

    GLuint fbo;

    std::unique_ptr<Texture> cTexArr[8];
    std::unique_ptr<Texture> dsTex;

private:
    std::vector<GLenum> drawBuffers;
    glm::uvec2 size;
};

}
