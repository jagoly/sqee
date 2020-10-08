#include <sqee/gl/VertexArray.hpp>

#include <sqee/debug/Assert.hpp>

#include <sqee/gl/Constants.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/Functions.hpp>

using namespace sq;

//============================================================================//

VertexArray::VertexArray()
{
    gl::CreateVertexArrays(1, &mHandle);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
{
    *this = std::move(other);
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
    std::swap(mHandle, other.mHandle);
    return *this;
}

VertexArray::~VertexArray() noexcept
{
    if (mHandle) gl::DeleteVertexArrays(1, &mHandle);
}

//============================================================================//

// todo: add SafeEnum version, merge size/type/normalize into one arguement

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
