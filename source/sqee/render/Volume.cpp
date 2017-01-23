#include <sqee/gl/Context.hpp>

#include <sqee/render/Volume.hpp>

using namespace sq;

//============================================================================//

Volume::Volume(const float* vertices, const uchar* indices, uchar vertexCount, uchar indexCount)

    : mVertexBuffer(gl::ARRAY_BUFFER), mIndexBuffer(gl::ELEMENT_ARRAY_BUFFER),
      mVertexCount(vertexCount), mIndexCount(indexCount)
{
    constexpr uint vertexSize = sizeof(GLfloat[3]);
    constexpr uint indexSize = sizeof(GLbyte);

    mVertexBuffer.allocate_constant(vertexCount * vertexSize, vertices);
    mIndexBuffer.allocate_constant(indexCount * indexSize, indices);

    mVAO.set_vertex_buffer(mVertexBuffer, 0u, vertexSize);
    mVAO.set_index_buffer(mIndexBuffer);

    mVAO.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);
}

//============================================================================//

void Volume::bind_and_draw(Context& context) const
{
    context.bind_VertexArray(mVAO);

    const auto count = GLsizei(mIndexCount);
    gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_BYTE, nullptr);
}
