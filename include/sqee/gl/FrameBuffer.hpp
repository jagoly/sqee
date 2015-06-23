#pragma once
#include <sqee/forward.hpp>

namespace sq {

class FrameBuffer : NonCopyable {
public:
    FrameBuffer();
    ~FrameBuffer();

    GLuint fbo;

    void attach(GLenum _attach, Texture2D& _tex);
    void attach(GLenum _attach, TextureArray& _tex, GLint _layer);
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
