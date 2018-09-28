// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

//============================================================================//

/// Tiny position-only mesh object.
class SQEE_API Volume final : private MoveOnly
{
public: //====================================================//

    /// Constructor.
    /// @param vertices pointer to array of vertices
    /// @param indices pointer to array of indices
    /// @param vertexCount number of vertices to use
    /// @param indexCount number of indices to use

    Volume ( const float* vertices, const uchar* indices,
             uint8_t vertexCount, uint8_t indexCount );

    //--------------------------------------------------------//

    /// Bind the vao to the context and draw the mesh.
    void bind_and_draw(Context& context) const;

private: //===================================================//

    VertexArray mVertexArray;

    FixedBuffer mVertexBuffer;
    FixedBuffer mIndexBuffer;

    const uint8_t mVertexCount;
    const uint8_t mIndexCount;
};

//============================================================================//

} // namespace sq
