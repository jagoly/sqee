#pragma once

#include <sqee/setup.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/gl/Textures.hpp>

namespace sq {

//============================================================================//

/// OpenGL Frame Buffer Object.
class FrameBuffer final : public MoveOnly
{
public: //====================================================//

    FrameBuffer(); ///< Constructor.

    FrameBuffer(FrameBuffer&& other); ///< Move Constructor.

    FrameBuffer& operator=(FrameBuffer&& other); ///< Move Assignment.

    ~FrameBuffer(); ///< Destructor.

    //--------------------------------------------------------//

    /// Attach an immutable Texture2D.
    void attach(GLenum attach, const Texture2D& tex);

    /// Attach an immutable TextureCube.
    void attach(GLenum attach, const TextureCube& tex, uint face);

    /// Attach an immutable TextureArray2D.
    void attach(GLenum attach, const TextureArray2D& tex, uint layer);

    /// Attach an immutable TextureArrayCube.
    void attach(GLenum attach, const TextureArrayCube& tex, uint layer, uint face);

    /// Attach an immutable TextureMulti.
    void attach(GLenum attach, const TextureMulti& tex);

    //--------------------------------------------------------//

    /// Set which buffers to draw to when using the fbo.
    void draw_buffers(std::initializer_list<GLenum> drawBufs);

    //--------------------------------------------------------//

    /// Blit this FrameBuffer directly to another.
    void blit(const FrameBuffer& other, Vec2U size, GLenum mask) const;

    /// Blit this FrameBuffer directly to another of a different size.
    void blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, GLenum mask, GLenum filter) const;

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    Context& mContext;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
