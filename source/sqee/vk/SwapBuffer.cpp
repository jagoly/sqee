#include <sqee/vk/SwapBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/vk/VulkanContext.hpp>
#include <sqee/vk/VulkanMemory.hpp>

using namespace sq;

//==============================================================================

SwapBuffer::SwapBuffer(SwapBuffer&& other)
{
    *this = std::move(other);
}

SwapBuffer& SwapBuffer::operator=(SwapBuffer&& other)
{
    std::swap(mStuff, other.mStuff);
    mPointer = other.mPointer;
    mSize = other.mSize;
    return *this;
}

SwapBuffer::~SwapBuffer()
{
    if (!mStuff.front.buffer) return;
    const auto& ctx = VulkanContext::get();
    mStuff.front.destroy(ctx);
    mStuff.back.destroy(ctx);
}

//==============================================================================

void SwapBuffer::initialise(size_t size, vk::BufferUsageFlags usage)
{
    SQASSERT(!mStuff.front.buffer, "buffer already initialised");

    const auto& ctx = VulkanContext::get();

    mStuff.front.initialise(ctx, size, usage, true);
    mStuff.back.initialise(ctx, size, usage, true);

    mPointer.front = mStuff.front.memory.map();
    mPointer.back = mStuff.back.memory.map();

    mSize = size;
}
