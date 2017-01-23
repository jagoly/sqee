#include <sqee/assert.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/UniformBuffer.hpp>

using namespace sq;

//============================================================================//

UniformBuffer::UniformBuffer() : mContext(Context::get()) {}

//============================================================================//

UniformBuffer::UniformBuffer(UniformBuffer&& other)
    : mContext(other.mContext)
{
    mContext.impl_reset_UniformBuffer(&other, this);

    mItemMap = std::move(other.mItemMap);
    mCurrentSize = other.mCurrentSize;

    mHandle = other.mHandle;
    other.mHandle = 0u;
}

//============================================================================//

UniformBuffer::~UniformBuffer()
{
    if (mHandle != 0u)
    {
        mContext.impl_reset_UniformBuffer(this);
        gl::DeleteBuffers(1, &mHandle);
        mHandle = 0u;
    }
}

//============================================================================//

void UniformBuffer::reserve(const string& name, uint size)
{
    mItemMap.emplace(name, Item{mCurrentSize, size * 4u});
    mCurrentSize += size * 4u;
}

//============================================================================//

void UniformBuffer::create_and_allocate()
{
    SQASSERT(mHandle == 0u, "UniformBuffer already created");

    gl::CreateBuffers(1, &mHandle);
    gl::NamedBufferStorage(mHandle, mCurrentSize, nullptr, gl::DYNAMIC_STORAGE_BIT);
}

//============================================================================//

void UniformBuffer::update(const string& name, const void* data)
{
    const Item& item = mItemMap.at(name);

    gl::NamedBufferSubData(mHandle, item.offset, item.size, data);
}

void UniformBuffer::update(const string& name, uint offset, uint size, const void* data)
{
    const Item& item = mItemMap.at(name);

    offset *= 4u; size *= 4u;
    SQASSERT(offset + size <= item.size, "range out of bounds");

    gl::NamedBufferSubData(mHandle, item.offset + offset, size, data);
}

void UniformBuffer::update(uint offset, uint size, const void* data)
{
    offset *= 4u; size *= 4u;
    SQASSERT(offset + size <= mCurrentSize, "range out of bounds");

    gl::NamedBufferSubData(mHandle, offset, size, data);
}
