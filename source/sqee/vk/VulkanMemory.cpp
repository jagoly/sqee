#include <sqee/vk/VulkanMemory.hpp>

#include <sqee/debug/Assert.hpp>

// details:
// - alignment is fixed for each block, so some space may be wasted
// - blocks never get freed, expectation is that memory will get reused
// - allocations bigger than MIN_BLOCK_SIZE get their own blocks
//
// todo:
// - add a method to explicitly free empty blocks
// - empty blocks should be reusable with different alignment
// - optimise finding usable chunks
// - non-fixed chunk alignment for less wasted space
// - unit tests

using namespace sq;

//============================================================================//

inline size_t impl_round_up(size_t size, size_t alignment)
{
    return ((size + alignment - 1u) / alignment) * alignment;
}

//============================================================================//

void VulkanAllocator::initialise(vk::Device device, uint32_t memTypeHost, uint32_t memTypeDevice, size_t maxItems)
{
    mDevice = device;
    mMemTypeHost = memTypeHost;
    mMemTypeDevice = memTypeDevice;

    mPoolStorage.reset(new FreeSlot[maxItems]);
    mNextFreeSlot = mPoolStorage.get();

    FreeSlot* iter = mNextFreeSlot;
    for (size_t i = 1u; i < maxItems; ++i)
    {
        iter->nextFreeSlot = std::next(iter);
        iter = iter->nextFreeSlot;
    }

    iter->nextFreeSlot = nullptr;
    mFinalSlot = iter;
}

void VulkanAllocator::destroy()
{
    for (auto& block : mHostMemoryBlocks)
    {
        SQASSERT(block->firstChunk->free && block->firstChunk->next == nullptr, "unfreed host memory");
        mDevice.free(block->memory);
    }

    for (auto& block : mDeviceMemoryBlocks)
    {
        SQASSERT(block->firstChunk->free && block->firstChunk->next == nullptr, "unfreed device memory");
        mDevice.free(block->memory);
    }
}

//============================================================================//

VulkanMemory VulkanAllocator::allocate(size_t alignment, size_t size, bool host)
{
    alignment = std::max(alignment, MIN_ALIGNMENT);

    auto& memoryBlocks = host ? mHostMemoryBlocks : mDeviceMemoryBlocks;
    const auto memoryTypeIndex = host ? mMemTypeHost : mMemTypeDevice;

    Chunk* chunk = nullptr;

    // look for any usable chunk in existing blocks
    for (auto& block : memoryBlocks)
    {
        if (block->alignment != alignment) continue;
        if (block->size < size) continue;

        chunk = impl_find_usable_chunk(size, *block->firstChunk);
        if (chunk != nullptr) break;
    }

    // no usable chunk, so allocate a new block
    if (chunk == nullptr)
    {
        auto& block = memoryBlocks.emplace_back(std::make_unique<MemoryBlock>());

        block->allocator = this;
        block->alignment = alignment;
        block->size = std::max(impl_round_up(size, alignment), MIN_BLOCK_SIZE);
        block->memory = mDevice.allocateMemory(vk::MemoryAllocateInfo(block->size, memoryTypeIndex));
        block->mapped = nullptr;
        block->mapCount = 0u;

        chunk = impl_new_chunk();
        chunk->block = block.get();
        chunk->offset = 0u;
        chunk->size = block->size;
        chunk->previous = nullptr;
        chunk->next = nullptr;

        block->firstChunk = chunk;
    }

    const size_t alignedSize = impl_round_up(size, alignment);

    // create new chunk in the space after
    if (alignedSize != chunk->size)
    {
        Chunk* next = impl_new_chunk();
        next->block = chunk->block;
        next->offset = chunk->offset + alignedSize;
        next->size = chunk->size - alignedSize;
        next->previous = chunk;
        next->next = chunk->next;

        chunk->next = next;
    }

    chunk->size = size;
    chunk->free = false;

    return VulkanMemory(chunk);
}

VulkanMemory VulkanAllocator::allocate(const vk::MemoryRequirements& requirements, bool host)
{
    return allocate(requirements.alignment, requirements.size, host);
}

//============================================================================//

VulkanAllocator::Chunk* VulkanAllocator::impl_find_usable_chunk(size_t size, Chunk& chunk)
{
    if (chunk.free == true && chunk.size >= size)
        return &chunk;

    if (chunk.next != nullptr)
        return impl_find_usable_chunk(size, *chunk.next);

    return nullptr;
}

VulkanAllocator::Chunk* VulkanAllocator::impl_new_chunk()
{
    SQASSERT(mNextFreeSlot != nullptr, "");

    auto result = &mNextFreeSlot->chunk;
    mNextFreeSlot = mNextFreeSlot->nextFreeSlot;

    result->free = true;
    result->mapped = false;

    return result;
}

void VulkanAllocator::impl_delete_chunk(Chunk* ptr)
{
    FreeSlot* slot = reinterpret_cast<FreeSlot*>(ptr);

    if (slot < mNextFreeSlot)
    {
        slot->nextFreeSlot = mNextFreeSlot;
        mNextFreeSlot = slot;
    }
    else
    {
        slot->nextFreeSlot = mNextFreeSlot->nextFreeSlot;
        mNextFreeSlot->nextFreeSlot = slot;
    }
}

//============================================================================//

void* VulkanMemory::map()
{
    SQASSERT(chunk->mapped == false, "memory already mapped");

    auto& block = *chunk->block;
    auto device = block.allocator->mDevice;

    if (block.mapped == nullptr)
        block.mapped = device.mapMemory(block.memory, 0u, block.size, {});

    ++block.mapCount;
    chunk->mapped = true;

    return static_cast<char*>(block.mapped) + chunk->offset;
}

void VulkanMemory::unmap()
{
    SQASSERT(chunk->mapped == true, "memory not mapped");

    auto& block = *chunk->block;
    auto device = block.allocator->mDevice;

    if (--block.mapCount == 0u)
    {
        device.unmapMemory(block.memory);
        block.mapped = nullptr;
    }

    chunk->mapped = false;
}

//============================================================================//

void VulkanMemory::free()
{
    SQASSERT(chunk != nullptr, "memory not allocated");

    if (chunk->mapped == true)
        unmap();

    chunk->size = impl_round_up(chunk->size, chunk->block->alignment);

    // merge with previous chunk
    if (chunk->previous != nullptr && chunk->previous->free == true)
    {
        auto previous = chunk->previous;

        chunk->offset = previous->offset;
        chunk->size += previous->size;
        chunk->previous = previous->previous;

        chunk->block->allocator->impl_delete_chunk(previous);

        if (chunk->previous != nullptr)
            chunk->previous->next = chunk;

        if (chunk->previous == nullptr)
            chunk->block->firstChunk = chunk;
    }

    // merge with next chunk
    if (chunk->next != nullptr && chunk->next->free == true)
    {
        auto next = chunk->next;

        chunk->size += next->size;
        chunk->next = next->next;

        chunk->block->allocator->impl_delete_chunk(next);

        if (chunk->next != nullptr)
            chunk->next->previous = chunk;
    }

    chunk->free = true;
    chunk = nullptr;
}
