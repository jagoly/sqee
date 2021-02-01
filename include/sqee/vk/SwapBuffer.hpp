// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/VulkanMemory.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

class SQEE_API SwapBuffer final : private MoveOnly
{
public: //====================================================//

    SwapBuffer() = default;

    SwapBuffer(SwapBuffer&& other);

    SwapBuffer& operator=(SwapBuffer&& other);

    ~SwapBuffer();

    //--------------------------------------------------------//

    /// Create and allocate buffers.
    void initialise(size_t size, vk::BufferUsageFlags usage);

    /// Swap front and back buffers.
    void swap();

    //--------------------------------------------------------//

    /// Access the front buffer object.
    vk::Buffer front() const { return mBuffer.front; }

    /// Access the back buffer object.
    vk::Buffer back() const { return mBuffer.back; }

    /// Access the mapped memory.
    void* map() { return mBufferPtr.front; }

    //--------------------------------------------------------//

    /// Get descriptor info for the front buffer.
    vk::DescriptorBufferInfo get_descriptor_info_front() const { return { mBuffer.front, 0u, mSize }; }

    /// Get descriptor info for the back buffer.
    vk::DescriptorBufferInfo get_descriptor_info_back() const { return { mBuffer.back, 0u, mSize }; }

private: //===================================================//

    size_t mSize = 0u;

    Swapper<vk::Buffer> mBuffer;
    Swapper<VulkanMemory> mBufferMem;
    Swapper<void*> mBufferPtr;
};

//============================================================================//

} // namespace sq
