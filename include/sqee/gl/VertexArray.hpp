#pragma once

#include <sqee/setup.hpp>

// Forward Declarations /////
namespace sq { class FixedBuffer; class DynamicBuffer; }

namespace sq {

/// A class for OpenGL vertex array objects
class VertexArray final : NonCopyable {
public:
    VertexArray(); ~VertexArray();

    /// Add a float attribute from a fixed size buffer
    void add_attribute(const FixedBuffer& _buffer, uint _index, uint _offs,
                       uint _stride, uint _size, GLenum _type, bool _normalize) const;

    /// Add a float attribute from a resizable buffer
    void add_attribute(const DynamicBuffer& _buffer, uint _index, uint _offs,
                       uint _stride, uint _size, GLenum _type, bool _normalize) const;

    /// Add an integer attribute from a fixed size buffer
    void add_attribute_I(const FixedBuffer& _buffer, uint _index, uint _offs,
                         uint _stride, uint _size, GLenum _type) const;

    /// Add an integer attribute from a resizable buffer
    void add_attribute_I(const DynamicBuffer& _buffer, uint _index, uint _offs,
                         uint _stride, uint _size, GLenum _type) const;

    /// Set a fixed-size buffer to use for indexed rendering
    void set_element_buffer(const FixedBuffer& _buffer) const;

    /// Set a resizable buffer to use for indexed rendering
    void set_element_buffer(const DynamicBuffer& _buffer) const;

    /// Bind the vertex array
    void bind() const;

    /// Get the OpenGL handle
    const GLuint& get_handle() const { return vao; }

private:
    /// The OpenGL handle
    GLuint vao = 0u;
};

}
