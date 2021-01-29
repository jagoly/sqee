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
    vk::Buffer front() const { return mBufferFront; }

    /// Access the back buffer object.
    vk::Buffer back() const { return mBufferBack; }

    /// Access the mapped memory.
    void* map() { return mBufferPtrFront; }

    //--------------------------------------------------------//

    /// Get descriptor info for the front buffer.
    vk::DescriptorBufferInfo get_descriptor_info_front() const { return { mBufferFront, 0u, mSize }; }

    /// Get descriptor info for the back buffer.
    vk::DescriptorBufferInfo get_descriptor_info_back() const { return { mBufferBack, 0u, mSize }; }

private: //===================================================//

    size_t mSize = 0u;

    vk::Buffer mBufferFront, mBufferBack;
    VulkanMemory mBufferMemFront, mBufferMemBack;
    void *mBufferPtrFront, *mBufferPtrBack;
};

//============================================================================//

} // namespace sq
