#pragma once

#include <sqee/setup.hpp>

namespace sq {

/// A class for resizable OpenGL buffer objects
class DynamicBuffer final : NonCopyable {
public:
    DynamicBuffer(GLenum _target);
    ~DynamicBuffer();

    /// Manually delete the GL object
    void delete_object();

    /// Allocate the buffer with constant data
    void allocate_constant(uint _size, const void* _data) const;

    /// Allocate the buffer with editable data
    void allocate_editable(uint _size, const void* _data) const;

    /// Update a section of the buffer with a pointer
    void update(uint _offs, uint _size, const void* _data) const;

    /// Bind the buffer
    void bind() const;

    /// The OpenGL handle
    GLuint buf = 0u;

private:
    const GLenum target;
};

}
