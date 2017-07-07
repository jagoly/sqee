#pragma once

#include <sqee/setup.hpp>
#include <sqee/gl/FixedBuffer.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Vertex Array Object.
class VertexArray final : public MoveOnly
{
public: //====================================================//

    VertexArray(); ///< Constructor.

    VertexArray(VertexArray&& other); ///< Move Constructor.

    VertexArray& operator=(VertexArray&& other); ///< Move Assignment.

    ~VertexArray(); ///< Destructor.

    //--------------------------------------------------------//

    /// Specify a floating point vertex attribute.
    ///
    /// @param index vertex shader input attribute index
    /// @param size number of components (1, 2, 3, 4)
    /// @param type OpenGL type of individual components
    /// @param normalize should input values be normalized
    /// @param offset offset to the first element in the vertex buffer

    void add_float_attribute(uint index, uint size, GLenum type, bool normalize, uint offset);

    //--------------------------------------------------------//

    /// Specify an integral vertex attribute.
    ///
    /// @param index vertex shader input attribute index
    /// @param size number of components (1, 2, 3, 4)
    /// @param type GL type of individual components
    /// @param offset offset to the first element in the vertex buffer

    void add_integer_attribute(uint index, uint size, GLenum type, uint offset);

    //--------------------------------------------------------//

    /// Specify the buffer to use for vertex data.
    void set_vertex_buffer(const FixedBuffer& buffer, uint stride);

    /// Specify the buffer to use for index data.
    void set_index_buffer(const FixedBuffer& buffer);

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    Context& mContext;

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
