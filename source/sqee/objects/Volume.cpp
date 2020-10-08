#include <sqee/objects/Volume.hpp>

using namespace sq;

//============================================================================//

Volume::Volume(const float* vertices, const uchar* indices, uint8_t vertexCount, uint8_t indexCount)
    : mVertexCount(vertexCount), mIndexCount(indexCount)
{
    mVertexBuffer.allocate_static(mVertexCount * sizeof(float[3]), vertices);
    mIndexBuffer.allocate_static(mIndexCount * sizeof(uchar), indices);

    mVertexArray.set_vertex_buffer(mVertexBuffer, sizeof(float[3]));
    mVertexArray.set_index_buffer(mIndexBuffer);

    mVertexArray.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);
}

//============================================================================//

void Volume::apply_to_context(Context& context) const
{
    context.bind_vertexarray(mVertexArray);
}

void Volume::draw(const Context& context) const
{
    context.draw_elements_u8(DrawPrimitive::Triangles, 0u, mIndexCount);
}
