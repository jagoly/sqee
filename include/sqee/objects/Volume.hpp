// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

namespace sq {

//============================================================================//

/// Tiny position-only mesh object.
class SQEE_API Volume final : private MoveOnly
{
public: //====================================================//

    /// Constructor.
    Volume(const float* vertices, const uchar* indices, uint8_t vertexCount, uint8_t indexCount);

    /// Bind the vao to the context.
    void apply_to_context(Context& context) const;

    /// Draw the volume.
    void draw(const Context& context) const;

private: //===================================================//

    VertexArray mVertexArray;

    FixedBuffer mVertexBuffer;
    FixedBuffer mIndexBuffer;

    const uint8_t mVertexCount;
    const uint8_t mIndexCount;
};

//============================================================================//

} // namespace sq
