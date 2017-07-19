#include <sqee/gl/Context.hpp>
#include <sqee/gl/VertexArray.hpp>

using namespace sq;

//============================================================================//

VertexArray::VertexArray() : mContext(Context::get())
{
    gl::CreateVertexArrays(1, &mHandle);
}

//============================================================================//

VertexArray::VertexArray(VertexArray&& other) : mContext(other.mContext)
{
    mContext.impl_reset_VertexArray(&other, this);
    mHandle = other.mHandle; other.mHandle = 0u;
}

VertexArray& VertexArray::operator=(VertexArray&& other)
{ std::swap(*this, other); return *this; }

//============================================================================//

VertexArray::~VertexArray()
{
    mContext.impl_reset_VertexArray(this);
    gl::DeleteVertexArrays(1, &mHandle);
}

//============================================================================//

void VertexArray::add_float_attribute(uint index, uint size, GLenum type, bool normalize, uint offset)
{
    gl::VertexArrayAttribFormat(mHandle, index, int(size), type, normalize, offset);
    gl::VertexArrayAttribBinding(mHandle, index, 0u);
    gl::EnableVertexArrayAttrib(mHandle, index);
}

void VertexArray::add_integer_attribute(uint index, uint size, GLenum type, uint offset)
{
    gl::VertexArrayAttribIFormat(mHandle, index, int(size), type, offset);
    gl::VertexArrayAttribBinding(mHandle, index, 0u);
    gl::EnableVertexArrayAttrib(mHandle, index);
}

//============================================================================//

void VertexArray::set_vertex_buffer(const FixedBuffer& buffer, uint stride)
{
    gl::VertexArrayVertexBuffer(mHandle, 0u, buffer.get_handle(), 0u, int(stride));
}

void VertexArray::set_index_buffer(const FixedBuffer& buffer)
{
    gl::VertexArrayElementBuffer(mHandle, buffer.get_handle());
}