#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// A class for fixed size OpenGL buffer objects
class FixedBuffer final : NonCopyable {
public:
    FixedBuffer(GLenum _target);
    ~FixedBuffer();

    /// Allocate the buffer with constant data
    void allocate_constant(uint _size, const void* _data);

    /// Allocate the buffer with editable data
    void allocate_editable(uint _size, const void* _data);

    /// Update a section of the buffer with a pointer
    void update(uint _offs, uint _size, const void* _data);

    /// Bind the buffer
    void bind() const;

    /// Get the size of the buffer
    uint get_size() const;

    /// The OpenGL handle
    GLuint buf = 0u;

private:
    const GLenum target;
};

}
