#include <sqee/gl/FixedBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

FixedBuffer::FixedBuffer()
{
    gl::CreateBuffers(1, &mHandle);
}

//============================================================================//

FixedBuffer::FixedBuffer(FixedBuffer&& other) noexcept
{
    mBufferSize = other.mBufferSize;
    mHandle = other.mHandle;

    other.mBufferSize = 0u;
    other.mHandle = 0u;
}

FixedBuffer& FixedBuffer::operator=(FixedBuffer&& other) noexcept
{ std::swap(*this, other); return *this; }

//============================================================================//

FixedBuffer::~FixedBuffer() noexcept
{
    gl::DeleteBuffers(1, &mHandle);
}

//============================================================================//

void FixedBuffer::allocate_constant(uint size, const void* data, GLenum flags)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::NONE | flags);
    mBufferSize = size;
}

void FixedBuffer::allocate_dynamic(uint size, const void* data, GLenum flags)
{
    gl::NamedBufferStorage(mHandle, size, data, gl::DYNAMIC_STORAGE_BIT | flags);
    mBufferSize = size;
}

void FixedBuffer::update(uint offset, uint size, const void* data)
{
    SQASSERT(offset + size <= mBufferSize, "buffer too small");
    gl::NamedBufferSubData(mHandle, offset, size, data);
}
