// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/Swapper.hpp>
#include <sqee/vk/Vulkan.hpp>
#include <sqee/vk/Wrappers.hpp>

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

    //--------------------------------------------------------//

    /// Swap front and back buffers.
    void swap_only() { mStuff.swap(); mPointer.swap(); }

    /// Map the front buffer's memory.
    std::byte* map_only() { return mPointer.front; };

    /// Map the front buffer's memory (const).
    const std::byte* map_only() const { return mPointer.front; };

    /// Swap front and back, then map front.
    std::byte* swap_map() { swap_only(); return map_only(); };

    //--------------------------------------------------------//

    /// Access the front buffer object.
    vk::Buffer front() const { return mStuff.front.buffer; }

    /// Access the back buffer object.
    vk::Buffer back() const { return mStuff.back.buffer; }

    /// Get info needed to update a descriptor set.
    Swapper<vk::DescriptorBufferInfo> get_descriptor_info() const
    {
        return { {mStuff.front.buffer, 0u, mSize}, {mStuff.back.buffer, 0u, mSize} };
    }

private: //===================================================//

    Swapper<BufferStuff> mStuff;
    Swapper<std::byte*> mPointer;

    size_t mSize = 0u;
};

//============================================================================//

} // namespace sq
