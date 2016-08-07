#pragma once

#include <sqee/builtins.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

namespace sq {

/// The SQEE Volume class
class Volume final : NonCopyable {
public:
    Volume(const float* _verts, const uchar* _elems,
           uchar _vertCount, uchar _elemCount);

    void bind_and_draw() const;

private:
    FixedBuffer vbo, ebo; VertexArray vao;
    const uint vertexCount, elementCount;
};

}
