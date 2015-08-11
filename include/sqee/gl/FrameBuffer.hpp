#pragma once
#include <sqee/forward.hpp>

namespace sq {

class FrameBuffer : NonCopyable {
public:
    FrameBuffer();
    ~FrameBuffer();

    GLuint fbo = 0u;

    void attach(GLenum _attach, TextureMut2D& _tex);
    void attach(GLenum _attach, TextureMutCube& _tex, uint _face);
    void attach(GLenum _attach, TextureMut2DArray& _tex, uint _index);
    void attach(GLenum _attach, TextureMutCubeArray& _tex, uint _face, uint _index);

    void bind(GLenum _target);
    void draw_bufs();
    void use();

protected:
    vector<GLenum> drawBufs;
    unordered_set<GLenum> bufSet;
    void add_buf(GLenum _attach);
};

}
