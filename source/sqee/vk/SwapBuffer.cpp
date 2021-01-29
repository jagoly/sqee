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
    std::swap(mBufferFront, other.mBufferFront);
    std::swap(mBufferBack, other.mBufferBack);
    std::swap(mBufferMemFront, other.mBufferMemFront);
    std::swap(mBufferMemBack, other.mBufferMemBack);
    mBufferPtrFront = other.mBufferPtrFront;
    mBufferPtrBack = other.mBufferPtrBack;
    return *this;
}

SwapBuffer::~SwapBuffer()
{
    const auto& ctx = VulkanContext::get();
    if (mBufferFront) ctx.device.destroy(mBufferFront);
    if (mBufferBack) ctx.device.destroy(mBufferBack);
    if (mBufferMemFront) mBufferMemFront.free();
    if (mBufferMemBack) mBufferMemBack.free();
}

//============================================================================//

void SwapBuffer::initialise(size_t size, vk::BufferUsageFlags usage)
{
    SQASSERT(mSize == 0u, "buffer already initialised");

    mSize = size;

    const auto& ctx = VulkanContext::get();

    std::tie(mBufferFront, mBufferMemFront) = vk_create_buffer(ctx, size, usage, true);
    std::tie(mBufferBack, mBufferMemBack) = vk_create_buffer(ctx, size, usage, true);

    mBufferPtrFront = mBufferMemFront.map();
    mBufferPtrBack = mBufferMemBack.map();
}

//============================================================================//

void SwapBuffer::swap()
{
    std::swap(mBufferFront, mBufferBack);
    std::swap(mBufferMemFront, mBufferMemBack);
    std::swap(mBufferPtrFront, mBufferPtrBack);
}
