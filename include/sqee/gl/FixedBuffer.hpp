// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

/// OpenGL Buffer Object.
class SQEE_API FixedBuffer final : private MoveOnly
{
public: //====================================================//

    FixedBuffer(); ///< Constructor.

    FixedBuffer(FixedBuffer&& other) noexcept; ///< Move Constructor.

    FixedBuffer& operator=(FixedBuffer&& other) noexcept; ///< Move Assignment.

    ~FixedBuffer() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Allocate the buffer with constant data.
    void allocate_constant(uint size, const void* data, GLenum flags = 0u);

    /// Allocate the buffer with dynamic data.
    void allocate_dynamic(uint size, const void* data, GLenum flags = 0u);

    /// Update a section of the buffer with a pointer.
    void update(uint offset, uint size, const void* data);

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    uint mBufferSize = 0u;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
