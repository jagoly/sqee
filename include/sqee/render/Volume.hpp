#pragma once

#include <sqee/builtins.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/gl/FixedBuffer.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// The SQEE Volume class
class Volume final : public MoveOnly
{
public:

    //========================================================//

    Volume(const float* vertices, const uchar* indices, uchar vertexCount, uchar indexCount);

    void bind_and_draw(Context& context) const;

private:

    //========================================================//

    VertexArray mVAO;
    FixedBuffer mVertexBuffer;
    FixedBuffer mIndexBuffer;

    const uint mVertexCount;
    const uint mIndexCount;
};

//============================================================================//

} // namespace sq
