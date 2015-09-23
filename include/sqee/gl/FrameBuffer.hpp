#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// A class for an OpenGL framebuffer
class FrameBuffer : NonCopyable {
public:
    FrameBuffer();
    ~FrameBuffer();

    /// Recreate FBO and clear draw buffers
    void revert_to_defaults();

    /// Attach an immutable 2D texture
    void attach(GLenum _attach, Texture2D& _tex);

    /// Attach a mutable 2D Texture
    void attach(GLenum _attach, TextureMut2D& _tex);

    /// Attach an immutable cubemap texture
    void attach(GLenum _attach, TextureCube& _tex, uint _face);

    /// Attach a mutable cubemap texture
    void attach(GLenum _attach, TextureMutCube& _tex, uint _face);

    /// Attach an immutable 2D array texture
    void attach(GLenum _attach, Texture2DArray& _tex, uint _index);

    /// Attach a mutable 2D array texture
    void attach(GLenum _attach, TextureMut2DArray& _tex, uint _index);

    /// Attach an immutable cubemap array texture
    void attach(GLenum _attach, TextureCubeArray& _tex, uint _index, uint _face);

    /// Attach a mutable cubemap array texture
    void attach(GLenum _attach, TextureMutCubeArray& _tex, uint _index, uint _face);

    /// Bind the FBO to the specified framebuffer target
    void bind(GLenum _target);

    /// Call gl::DrawBuffers for the active buffers
    void draw_bufs();

    /// Call bind(gl::FRAMEBUFFER) and draw_bufs()
    void use();

    /// The OpenGL handle
    GLuint fbo = 0u;

protected:
    vector<GLenum> drawBufs;
    unordered_set<GLenum> bufSet;
    void add_buf(GLenum _attach);
};

}
