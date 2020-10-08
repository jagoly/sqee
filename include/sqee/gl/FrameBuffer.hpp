// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>
#include <sqee/gl/SafeEnums.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Texture2D;
class TextureArray;
class TextureCube;
class TextureMulti;

//============================================================================//

/// OpenGL Frame Buffer Object.
class SQEE_API FrameBuffer final : private MoveOnly
{
public: //====================================================//

    FrameBuffer(); ///< Constructor.

    FrameBuffer(FrameBuffer&& other) noexcept; ///< Move Constructor.

    FrameBuffer& operator=(FrameBuffer&& other) noexcept; ///< Move Assignment.

    ~FrameBuffer() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Attach an immutable Texture2D.
    void attach(FboAttach attach, const Texture2D& tex);

    /// Attach an immutable TextureCube.
    void attach(FboAttach attach, const TextureCube& tex, uint face);

    /// Attach an immutable TextureArray.
    void attach(FboAttach attach, const TextureArray& tex, uint layer);

    /// Attach an immutable TextureMulti.
    void attach(FboAttach attach, const TextureMulti& tex);

    //--------------------------------------------------------//

    /// Set which buffers to draw to when using the fbo.
    void draw_buffers(std::initializer_list<FboAttach> drawBufs);

    //--------------------------------------------------------//

    /// Blit this FrameBuffer directly to another.
    void blit(const FrameBuffer& other, Vec2U size, BlitMask mask) const;

    /// Blit this FrameBuffer directly to another of a different size.
    void blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, BlitMask mask, bool filter) const;

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
