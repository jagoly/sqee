#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

/// Fixed-Size OpenGL Buffer Object
class FixedBuffer final : public MoveOnly
{
public:

    //========================================================//

    /// Constructor
    FixedBuffer(GLenum target);

    /// Move Constructor
    FixedBuffer(FixedBuffer&& other);

    /// Destructor
    ~FixedBuffer();

    //========================================================//

    /// Allocate the buffer with constant data
    void allocate_constant(uint size, const void* data);

    /// Allocate the buffer with editable data
    void allocate_editable(uint size, const void* data);

    /// Update a section of the buffer with a pointer
    void update(uint offset, uint size, const void* data);

    /// Bind the buffer
    void bind() const;

    //========================================================//

    /// Get the OpenGL object handle
    GLuint get_handle() const { return mHandle; }

private:

    //========================================================//

    const GLenum mTarget;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
