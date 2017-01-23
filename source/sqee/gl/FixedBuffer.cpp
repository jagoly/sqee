#include <sqee/redist/gl_ext_4_2.hpp>

#include <sqee/gl/FixedBuffer.hpp>

using namespace sq;

//============================================================================//

FixedBuffer::FixedBuffer(GLenum target)
    : mTarget(target)
{
    gl::CreateBuffers(1, &mHandle);
}

FixedBuffer::FixedBuffer(FixedBuffer&& other)
    : mTarget(other.mTarget)
{
    mHandle = other.mHandle;
    other.mHandle = 0u;
}

FixedBuffer::~FixedBuffer()
{
    gl::DeleteBuffers(1, &mHandle);
}

//============================================================================//

void FixedBuffer::allocate_constant(uint size, const void* data)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::NONE);
}

void FixedBuffer::allocate_editable(uint size, const void* data)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::DYNAMIC_STORAGE_BIT);
}

void FixedBuffer::update(uint offset, uint size, const void* data)
{
    gl::NamedBufferSubData(mHandle, offset, size, data);
}

//============================================================================//

void FixedBuffer::bind() const
{
    gl::BindBuffer(mTarget, mHandle);
}
