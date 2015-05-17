#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Framebuffer : NonCopyable {
public:
    Framebuffer();
    ~Framebuffer();

    GLuint fbo;

    void attach(GLenum _attach, Texture2D& _tex);
    void attach(GLenum _attach, Texture2DArray& _tex, GLint _layer);
    void attach(GLenum _attach, TextureCube& _tex, GLint _layer);

    void bind(GLenum _target);
    void draw_bufs();
    void use();

protected:
    vector<GLenum> drawBufs;
    unordered_set<GLenum> bufSet;
    void add_buf(GLenum _attach);
};

}
