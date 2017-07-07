#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

/// OpenGL Buffer Object.
class FixedBuffer final : public MoveOnly
{
public: //====================================================//

    /// Constructor.
    FixedBuffer();

    /// Move Constructor.
    FixedBuffer(FixedBuffer&& other);

    /// Move Assignment.
    FixedBuffer& operator=(FixedBuffer&& other);

    /// Destructor.
    ~FixedBuffer();

    //--------------------------------------------------------//

    /// Allocate the buffer with constant data.
    void allocate_constant(uint size, const void* data);

    /// Allocate the buffer with dynamic data.
    void allocate_dynamic(uint size, const void* data);

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
