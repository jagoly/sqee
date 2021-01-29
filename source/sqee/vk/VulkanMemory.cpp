#include <sqee/vk/VulkanMemory.hpp>

#include <sqee/debug/Assert.hpp>

// todo:
// - alignment is fixed for each block, so lots of mem is wasted
// - minFailSize is probably dumb premature optimisation, if it even works
// - blocks never get freed
// - needs unit tests

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
        //SQASSERT(block->firstChunk->free && block->firstChunk->next == nullptr, "unfreed host memory");
        mDevice.free(block->memory);
    }

    for (auto& block : mDeviceMemoryBlocks)
    {
        //SQASSERT(block->firstChunk->free && block->firstChunk->next == nullptr, "unfreed device memory");
        mDevice.free(block->memory);
    }
}

//============================================================================//

VulkanMemory VulkanAllocator::allocate(size_t alignment, size_t size, bool host)
{
    //SQASSERT(size <= MEMORY_BLOCK_SIZE, ""); // todo: just allocate a larger block

    alignment = std::max(alignment, MIN_ALIGNMENT);

    auto& memoryBlocks = host ? mHostMemoryBlocks : mDeviceMemoryBlocks;
    const auto memoryTypeIndex = host ? mMemTypeHost : mMemTypeDevice;

    Chunk* chunk = nullptr;

    // look for any usable chunk in existing blocks
    for (auto& block : memoryBlocks)
    {
        if (block->size < size) continue;
        if (block->alignment != alignment) continue;
        if (block->minFailSize <= size) continue;

        chunk = impl_find_usable_chunk(size, *block->firstChunk);
        if (chunk != nullptr) break;

        // skip this block for future allocations
        block->minFailSize = std::min(block->minFailSize, size);
    }

    // no usable chunk, so allocate a new block
    if (chunk == nullptr)
    {
        auto& block = memoryBlocks.emplace_back(std::make_unique<MemoryBlock>());

        block->allocator = this;
        block->alignment = alignment;
        block->size = impl_round_up(size, MIN_BLOCK_SIZE);
        block->memory = mDevice.allocateMemory(vk::MemoryAllocateInfo(block->size, memoryTypeIndex));

        chunk = impl_new_chunk();
        chunk->block = block.get();
        chunk->offset = 0u;
        chunk->size = block->size;
        chunk->previous = nullptr;
        chunk->next = nullptr;
        chunk->free = true;

        block->firstChunk = chunk;
        block->minFailSize = UINT64_MAX;
        block->mapped = nullptr;
        block->mapCount = 0u;
    }

//    const size_t alignedOffset = impl_round_up(chunk->offset, align);

//    // create new chunk in the space before
//    if (alignedOffset != chunk->offset)
//    {
//        Chunk* previous = impl_new_chunk();
//        previous->block = chunk->block;
//        previous->offset = chunk->offset;
//        previous->size = chunk->offset - alignedOffset;
//        previous->previous = chunk->previous;
//        previous->next = chunk;
//        previous->free = true;
//        previous->mapped = false;

//        chunk->previous = previous;
//    }

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
        next->free = true;
        next->mapped = false;

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

VulkanAllocator::Chunk* VulkanAllocator::impl_find_usable_chunk(size_t size, VulkanAllocator::Chunk& chunk)
{
    //const size_t alignedOffset = impl_round_up(chunk.offset, align);
    //const size_t usableSize = chunk.size + chunk.offset - alignedOffset;

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

    return result;
}

void VulkanAllocator::impl_delete_chunk(VulkanAllocator::Chunk* ptr)
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
    SQASSERT(mChunk->mapped == false, "");

    auto& block = *mChunk->block;
    auto device = block.allocator->mDevice;

    if (block.mapped == nullptr)
        block.mapped = device.mapMemory(block.memory, 0u, block.size, {});

    ++block.mapCount;
    mChunk->mapped = true;

    return static_cast<char*>(block.mapped) + mChunk->offset;
}

void VulkanMemory::unmap()
{
    SQASSERT(mChunk->mapped == true, "");

    auto& block = *mChunk->block;
    auto device = block.allocator->mDevice;

    if (--block.mapCount == 0u)
    {
        device.unmapMemory(block.memory);
        block.mapped = nullptr;
    }

    mChunk->mapped = false;
}

//============================================================================//

void VulkanMemory::free()
{
    SQASSERT(mChunk != nullptr, "");

    if (mChunk->mapped == true)
        unmap();

    mChunk->size = impl_round_up(mChunk->size, mChunk->block->alignment);

    // merge with previous chunk
    if (mChunk->previous != nullptr && mChunk->previous->free == true)
    {
        auto previous = mChunk->previous;
        mChunk->offset = previous->offset;
        mChunk->size += previous->size;
        mChunk->previous = previous->previous;
        mChunk->block->allocator->impl_delete_chunk(previous);

        if (mChunk->previous == nullptr)
            mChunk->block->firstChunk = mChunk;
    }

    // merge with next chunk
    if (mChunk->next != nullptr && mChunk->next->free == true)
    {
        auto next = mChunk->next;
        mChunk->size += next->size;
        mChunk->next = next->next;
        mChunk->block->allocator->impl_delete_chunk(next);
    }

    mChunk->block->minFailSize = std::max(mChunk->block->minFailSize, mChunk->size + 1u);

    mChunk->free = true;
    mChunk = nullptr;
}
