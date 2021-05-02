#include <sqee/vk/SwapBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkanContext.hpp>

using namespace sq;

//============================================================================//

SwapBuffer::SwapBuffer(SwapBuffer&& other)
{
    *this = std::move(other);
}

SwapBuffer& SwapBuffer::operator=(SwapBuffer&& other)
{
    mSize = other.mSize;
    std::swap(mBuffer, other.mBuffer);
    std::swap(mBufferMem, other.mBufferMem);
    mBufferPtr = other.mBufferPtr;
    return *this;
}

SwapBuffer::~SwapBuffer()
{
    const auto& ctx = VulkanContext::get();
    if (mBuffer.front) ctx.device.destroy(mBuffer.front);
    if (mBuffer.back) ctx.device.destroy(mBuffer.back);
    if (mBufferMem.front) mBufferMem.front.free();
    if (mBufferMem.back) mBufferMem.back.free();
}

//============================================================================//

void SwapBuffer::initialise(size_t size, vk::BufferUsageFlags usage)
{
    SQASSERT(mSize == 0u, "buffer already initialised");

    mSize = size;

    const auto& ctx = VulkanContext::get();

    std::tie(mBuffer.front, mBufferMem.front) = vk_create_buffer(ctx, size, usage, true);
    std::tie(mBuffer.back, mBufferMem.back) = vk_create_buffer(ctx, size, usage, true);

    mBufferPtr.front = mBufferMem.front.map();
    mBufferPtr.back = mBufferMem.back.map();
}
