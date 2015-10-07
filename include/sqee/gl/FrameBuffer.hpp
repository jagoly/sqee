#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// A class for an OpenGL framebuffer
class FrameBuffer final : NonCopyable {
public:
    FrameBuffer();
    ~FrameBuffer();

    /// Attach an immutable 2D texture
    void attach(GLenum _attach, const Texture2D& _tex) const;

    /// Attach a mutable 2D Texture
    void attach(GLenum _attach, TextureMut2D& _tex) const;

    /// Attach an immutable cubemap texture
    void attach(GLenum _attach, const TextureCube& _tex, uint _face) const;

    /// Attach a mutable cubemap texture
    void attach(GLenum _attach, TextureMutCube& _tex, uint _face) const;

    /// Attach an immutable 2D array texture
    void attach(GLenum _attach, const Texture2DArray& _tex, uint _index) const;

    /// Attach a mutable 2D array texture
    void attach(GLenum _attach, TextureMut2DArray& _tex, uint _index) const;

    /// Attach an immutable cubemap array texture
    void attach(GLenum _attach, const TextureCubeArray& _tex, uint _index, uint _face) const;

    /// Attach a mutable cubemap array texture
    void attach(GLenum _attach, TextureMutCubeArray& _tex, uint _index, uint _face) const;

    /// Set which buffers to draw to when using the framebuffer
    void draw_buffers(initializer_list<GLenum> _drawBufs) const;

    /// Bind the fbo to gl::FRAMEBUFFER
    void bind() const;

    /// Blit this framebuffer directly to another
    void blit(const FrameBuffer& _other, uvec2 _sizeA, uvec2 _sizeB, GLenum _mask, GLenum _filter) const;

    /// The OpenGL handle
    GLuint fbo = 0u;
};

}
