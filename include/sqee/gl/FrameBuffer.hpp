#pragma once

#include <initializer_list>

#include <sqee/maths/Vectors.hpp>
#include <sqee/gl/Textures.hpp>

namespace sq {

//============================================================================//

/// OpenGL FrameBuffer Object
class FrameBuffer final : public MoveOnly
{
public:

    //========================================================//

    /// Constructor
    FrameBuffer();

    /// Move Constructor
    FrameBuffer(FrameBuffer&& other);

    /// Destructor
    ~FrameBuffer();

    //========================================================//

    /// Attach an immutable 2D texture
    void attach(GLenum attach, const Texture2D& tex);

    /// Attach an immutable cubemap texture
    void attach(GLenum attach, const TextureCube& tex, uint face);

    /// Attach an immutable 2D array texture
    void attach(GLenum attach, const TextureArray2D& tex, uint layer);

    /// Attach an immutable cubemap array texture
    void attach(GLenum attach, const TextureArrayCube& tex, uint layer, uint face);

    //========================================================//

    /// Set which buffers to draw to when using the framebuffer
    void draw_buffers(std::initializer_list<GLenum> drawBufs);

    //========================================================//

    /// Blit this framebuffer directly to another
    void blit(const FrameBuffer& other, Vec2U size, GLenum mask) const;

    /// Blit this framebuffer directly to another of a different size
    void blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, GLenum mask, GLenum filter) const;

    //========================================================//

    /// Get the OpenGL object handle
    GLuint get_handle() const { return mHandle; }

private:

    //========================================================//

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
