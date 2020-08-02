#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

UniformBuffer::UniformBuffer() : mContext(Context::get()) {}

//============================================================================//

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
    : mContext(other.mContext)
{
    mContext.impl_reset_UniformBuffer(&other, this);

    mBufferSize = other.mBufferSize;
    mHandle = other.mHandle;

    other.mBufferSize = 0u;
    other.mHandle = 0u;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{ std::swap(*this, other); return *this; }

//============================================================================//

UniformBuffer::~UniformBuffer() noexcept
{
    mContext.impl_reset_UniformBuffer(this);

    if (mHandle) gl::DeleteBuffers(1, &mHandle);
}

//============================================================================//

void UniformBuffer::create_and_allocate(uint size)
{
    SQASSERT(mHandle == 0u, "UniformBuffer already created");

    gl::CreateBuffers(1, &mHandle);
    gl::NamedBufferStorage(mHandle, size, nullptr, gl::DYNAMIC_STORAGE_BIT);

    mBufferSize = size;
}

//============================================================================//

void UniformBuffer::update(uint offset, uint size, const void* data)
{
    SQASSERT(offset + size <= mBufferSize, "out of range");

    gl::NamedBufferSubData(mHandle, offset, size, data);
}
