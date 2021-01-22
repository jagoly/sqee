#pragma once

#include <sqee/setup.hpp>

#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

class VulkanMemory;

class SQEE_API VulkanAllocator : private NonCopyable
{
public: //====================================================//

    constexpr static size_t MEMORY_BLOCK_SIZE = 1024 * 1024 * 32;
    constexpr static size_t CHUNK_ALIGNMENT = 256;

    static_assert(MEMORY_BLOCK_SIZE % CHUNK_ALIGNMENT == 0u, "");

    //--------------------------------------------------------//

    void initialise(vk::Device device, uint32_t memTypeHost, uint32_t memTypeDevice, size_t maxItems);

    void destroy();

    VulkanMemory allocate(size_t alignment, size_t size, bool host);

    VulkanMemory allocate(const vk::MemoryRequirements& requirements, bool host);

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
        vk::DeviceMemory memory;
        Chunk* firstChunk;
        size_t minFailSize;
        void* mapped;
        size_t mapCount;
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

    VulkanMemory() : mChunk(nullptr) {}

    VulkanMemory(VulkanAllocator::Chunk* chunk) : mChunk(chunk) {}

    //--------------------------------------------------------//

    size_t get_offset() { return mChunk->offset; }

    size_t get_size() { return mChunk->size; }

    vk::DeviceMemory get_memory() { return mChunk->block->memory; }

    //--------------------------------------------------------//

    void* map();

    void unmap();

    void free();

private: //===================================================//

    VulkanAllocator::Chunk* mChunk;
};

//============================================================================//

} // namespace sq
