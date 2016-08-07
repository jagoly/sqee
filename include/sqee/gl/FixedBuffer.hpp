#pragma once

#include <sqee/setup.hpp>

namespace sq {

/// A class for fixed size OpenGL buffer objects
class FixedBuffer final : NonCopyable {
public:
    FixedBuffer(GLenum _target);
    ~FixedBuffer();

    /// Allocate the buffer with constant data
    void allocate_constant(uint _size, const void* _data) const;

    /// Allocate the buffer with editable data
    void allocate_editable(uint _size, const void* _data) const;

    /// Update a section of the buffer with a pointer
    void update(uint _offset, uint _size, const void* _data) const;

    /// Bind the buffer
    void bind() const;

    /// Get the OpenGL handle
    const GLuint& get_handle() const { return buf; }

private:
    /// The OpenGL handle
    GLuint buf = 0u;

    const GLenum target;
};

}
