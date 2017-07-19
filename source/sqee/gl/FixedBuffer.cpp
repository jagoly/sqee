#include <sqee/redist/gl_loader.hpp>

#include <sqee/gl/FixedBuffer.hpp>

using namespace sq;

//============================================================================//

FixedBuffer::FixedBuffer()
{
    gl::CreateBuffers(1, &mHandle);
}

//============================================================================//

FixedBuffer::FixedBuffer(FixedBuffer&& other)
{
    mBufferSize = other.mBufferSize;
    mHandle = other.mHandle;

    other.mBufferSize = 0u;
    other.mHandle = 0u;
}

FixedBuffer& FixedBuffer::operator=(FixedBuffer&& other)
{ std::swap(*this, other); return *this; }

//============================================================================//

FixedBuffer::~FixedBuffer()
{
    gl::DeleteBuffers(1, &mHandle);
}

//============================================================================//

void FixedBuffer::allocate_constant(uint size, const void* data)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::NONE);
}

void FixedBuffer::allocate_dynamic(uint size, const void* data)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::DYNAMIC_STORAGE_BIT);
}

void FixedBuffer::update(uint offset, uint size, const void* data)
{
    gl::NamedBufferSubData(mHandle, offset, size, data);
}