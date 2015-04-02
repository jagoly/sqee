#pragma once
#include <sqee/forward.hpp>

#include <set>
#include <vector>

namespace sq {

class Framebuffer : NonCopyable {
public:
    Framebuffer();

    GLuint fbo;

    void attach(GLenum _attach, Texture& _tex);
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
