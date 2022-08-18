#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

class VulkanMemory;

class SQEE_API VulkanAllocator : NonCopyable
{
public: //====================================================//

    constexpr static size_t MIN_BLOCK_SIZE = 1024u * 1024u * 4u;
    constexpr static size_t MIN_ALIGNMENT = 256u;

    static_assert(MIN_BLOCK_SIZE % MIN_ALIGNMENT == 0u, "");

    //--------------------------------------------------------//

    void initialise(vk::Device device, uint32_t memTypeHost, uint32_t memTypeDevice, size_t maxItems);

    void destroy();

    VulkanMemory allocate(size_t alignment, size_t size, bool host);

    VulkanMemory allocate(const vk::MemoryRequirements& requirements, bool host);

    void free_empty_blocks();

    size_t get_memory_usage(bool host) const;

private: //===================================================//

    struct MemoryBlock;

    struct Chunk
    {
        MemoryBlock* block;
        size_t offset;
        size_t size;
        Chunk* previous;
        Chunk* next;
        bool free;
        bool mapped;
    };

    struct MemoryBlock
    {
        VulkanAllocator* allocator;
        size_t alignment;
        size_t size;
        vk::DeviceMemory memory;
        std::byte* mapped;
        size_t mapCount;
        Chunk* firstChunk;
    };

    union FreeSlot
    {
        FreeSlot* nextFreeSlot;
        Chunk chunk;
    };

    //--------------------------------------------------------//

    Chunk* impl_find_usable_chunk(size_t size, Chunk& chunk);

    Chunk* impl_new_chunk();

    void impl_delete_chunk(Chunk* ptr);

    //--------------------------------------------------------//

    vk::Device mDevice;

    uint32_t mMemTypeHost;
    uint32_t mMemTypeDevice;

    std::unique_ptr<FreeSlot[]> mPoolStorage;

    FreeSlot* mNextFreeSlot;
    FreeSlot* mFinalSlot;

    std::vector<std::unique_ptr<MemoryBlock>> mHostMemoryBlocks;
    std::vector<std::unique_ptr<MemoryBlock>> mDeviceMemoryBlocks;

    friend VulkanMemory;
};

//============================================================================//

class SQEE_API VulkanMemory
{
public: //====================================================//

    VulkanMemory() : chunk(nullptr) {}

    VulkanMemory(VulkanAllocator::Chunk* _chunk) : chunk(_chunk) {}

    //--------------------------------------------------------//

    size_t get_offset() { return chunk->offset; }

    size_t get_size() { return chunk->size; }

    vk::DeviceMemory get_memory() { return chunk->block->memory; }

    //--------------------------------------------------------//

    std::byte* map();

    void unmap();

    void free();

    //--------------------------------------------------------//

    operator bool() const { return chunk != nullptr; }

private: //===================================================//

    VulkanAllocator::Chunk* chunk;
};

//============================================================================//

} // namespace sq
