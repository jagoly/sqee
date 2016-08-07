#pragma once

#include <initializer_list>

#include <sqee/maths/Vectors.hpp>

// Forward Declarations /////
namespace sq { class Texture2D; class TextureCube; class Texture2DArray; class TextureCubeArray; }

namespace sq {

/// A class for an OpenGL framebuffer
class FrameBuffer final : NonCopyable {
public:
    FrameBuffer(); ~FrameBuffer();

    /// Attach an immutable 2D texture
    void attach(GLenum _attach, const Texture2D& _tex);

    /// Attach an immutable cubemap texture
    void attach(GLenum _attach, const TextureCube& _tex, uint _face);

    /// Attach an immutable 2D array texture
    void attach(GLenum _attach, const Texture2DArray& _tex, uint _index);

    /// Attach an immutable cubemap array texture
    void attach(GLenum _attach, const TextureCubeArray& _tex, uint _index, uint _face);

    /// Set which buffers to draw to when using the framebuffer
    void draw_buffers(std::initializer_list<GLenum> _drawBufs);

    /// Blit this framebuffer directly to another
    void blit(const FrameBuffer& _other, Vec2U _size, GLenum _mask) const;

    /// Blit this framebuffer directly to another of a different size
    void blit(const FrameBuffer& _other, Vec2U _sizeA, Vec2U _sizeB, GLenum _mask, GLenum _filter) const;

    /// Get the OpenGL handle
    const GLuint& get_handle() const { return fbo; }

private:
    /// The OpenGL handle
    GLuint fbo = 0u;
};

}
