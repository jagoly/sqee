#pragma once

#include <sqee/gl/FixedBuffer.hpp>

namespace sq {

//============================================================================//

/// OpenGL Vertex Array Object
class VertexArray final : public MoveOnly
{
public:

    //========================================================//

    /// Constructor
    VertexArray();

    /// Move Constructor
    VertexArray(VertexArray&& other);

    /// Destructor
    ~VertexArray();

    //========================================================//

    /// specify a floating point vertex attribute
    void add_float_attribute(uint index, uint size, GLenum type, bool normalize, uint offset);

    /// specify an integral vertex attribute
    void add_integer_attribute(uint index, uint size, GLenum type, uint offset);

    /// set a fixed-size buffer to use for vertices
    void set_vertex_buffer(const FixedBuffer& buffer, uint offset, uint stride);

    /// set a fixed-size buffer to use for indices
    void set_index_buffer(const FixedBuffer& buffer);

    //========================================================//

    /// Get the OpenGL handle
    GLuint get_handle() const { return mHandle; }

private:

    //========================================================//

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
