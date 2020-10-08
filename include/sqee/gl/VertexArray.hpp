// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/gl/Types.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class FixedBuffer;

//============================================================================//

/// OpenGL Vertex Array Object.
class SQEE_API VertexArray final : private MoveOnly
{
public: //====================================================//

    VertexArray(); ///< Constructor.

    VertexArray(VertexArray&& other) noexcept; ///< Move Constructor.

    VertexArray& operator=(VertexArray&& other) noexcept; ///< Move Assignment.

    ~VertexArray() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Specify a floating point vertex attribute.
    void add_float_attribute(uint index, uint size, GLenum type, bool normalize, uint offset);

    /// Specify an integral vertex attribute.
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

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
