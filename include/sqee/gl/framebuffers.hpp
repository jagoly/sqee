#pragma once
#include <sqee/forward.hpp>

#include <set>

namespace sq {

class Framebuffer : NonCopyable {
public:
    Framebuffer();

    GLuint fbo;

    void attach(GLenum _attach, Texture2D& _tex);
    void attach(GLenum _attach, Texture2DArray& _tex, GLint _layer);
    void attach(GLenum _attach, TextureCube& _tex, GLint _layer);

    void bind();
    void draw_bufs();
    bool check();
    void use();

protected:
    vector<GLenum> drawBufs;
    std::set<GLenum> bufSet;
    void add_buf(GLenum _attach);
};

}
