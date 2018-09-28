// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/render/Volume.hpp>

#include <sqee/gl/Context.hpp>

#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

Volume::Volume ( const float* vertices, const uchar* indices,
                 uint8_t vertexCount, uint8_t indexCount )

    : mVertexCount(vertexCount), mIndexCount(indexCount)
{
    constexpr uint vertexSize = sizeof(GLfloat[3]);
    constexpr uint indexSize = sizeof(GLubyte);

    mVertexBuffer.allocate_constant(vertexCount * vertexSize, vertices);
    mIndexBuffer.allocate_constant(indexCount * indexSize, indices);

    mVertexArray.set_vertex_buffer(mVertexBuffer, vertexSize);
    mVertexArray.set_index_buffer(mIndexBuffer);

    mVertexArray.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);
}

//============================================================================//

void Volume::bind_and_draw(Context& context) const
{
    context.bind_VertexArray(mVertexArray);

    gl::DrawElements(gl::TRIANGLES, int(mIndexCount), gl::UNSIGNED_BYTE, nullptr);
}
