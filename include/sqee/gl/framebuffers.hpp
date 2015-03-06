#pragma once
#include "forward.hpp"

#include <set>
#include <vector>

#include "gl/textures.hpp"

namespace sq {

//class Framebuffer : NonCopyable {
//public:
//    Framebuffer();

//    void create_colour(uint _id, GLenum _format, GLenum _iFormat, Texture::Preset _preset);
//    void create_depth(GLenum _format, GLenum _iFormat, Texture::Preset _preset);

//    GLuint fbo;
//    std::unique_ptr<Texture> cTexArr[8];
//    std::unique_ptr<Texture> dsTex;

//    void resize(glm::uvec2 _size);
//    void use();
//    void useVP();

//protected:
//    std::vector<GLenum> drawBuffers;
//    glm::uvec2 size;
//};


class Framebuffer : NonCopyable {
public:
    Framebuffer();

    GLuint fbo;

    void attach_2d(GLenum _attach, Texture& _tex);
    void attach_layer(GLenum _attach, Texture& _tex, GLint _layer);

    void bind();
    bool check();
    void use();

protected:
    std::vector<GLenum> drawBufs;
    std::set<GLenum> bufSet;
    void add_buf(GLenum _attach);
};

}
