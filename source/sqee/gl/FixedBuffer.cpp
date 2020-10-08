#include <sqee/gl/FixedBuffer.hpp>

#include <sqee/debug/Assert.hpp>

#include <sqee/gl/Constants.hpp>
#include <sqee/gl/Functions.hpp>

using namespace sq;

//============================================================================//

FixedBuffer::FixedBuffer()
{
    gl::CreateBuffers(1, &mHandle);
}

FixedBuffer::FixedBuffer(FixedBuffer&& other) noexcept
{
    *this = std::move(other);
}

FixedBuffer& FixedBuffer::operator=(FixedBuffer&& other) noexcept
{
    std::swap(mHandle, other.mHandle);
    return *this;
}

FixedBuffer::~FixedBuffer() noexcept
{
    if (mHandle) gl::DeleteBuffers(1, &mHandle);
}

//============================================================================//

void FixedBuffer::allocate_static(uint size, const void* data)
{
    SQASSERT(data != nullptr, "data must be given");
    gl::NamedBufferStorage(mHandle, ptrdiff_t(size), data, gl::NONE);
}

void FixedBuffer::allocate_dynamic(uint size)
{
    gl::NamedBufferStorage(mHandle, ptrdiff_t(size), nullptr, gl::DYNAMIC_STORAGE_BIT);
}

//============================================================================//

void FixedBuffer::update(uint offset, uint size, const void* data)
{
    gl::NamedBufferSubData(mHandle, ptrdiff_t(offset), ptrdiff_t(size), data);
}
