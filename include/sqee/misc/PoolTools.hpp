#pragma once

#include <sqee/builtins.hpp>
#include <sqee/assert.hpp>

#include <sqee/misc/Algorithms.hpp>

namespace sq {

//============================================================================//

/// Fast eight byte string type.
struct TinyString
{
    constexpr TinyString(const char* str)
    {
        for (int i = 0; i < 7 && str[i] != 0; ++i)
            data.str[i] = str[i];
    }

    TinyString(const TinyString& other) = default;

    constexpr bool operator==(TinyString other) const
    {
        return data.u64 == other.data.u64;
    }

    constexpr const char* c_str() const
    {
        return data.str;
    }

    union { uint64_t u64 = 0u; char str[8]; } data;
};

//============================================================================//

/// A Simple Pool Allocator.
template <class Type>
class PoolAllocator : sq::NonCopyable
{
public: //====================================================//

    static constexpr size_t Size = sizeof(Type);

    //--------------------------------------------------------//

    PoolAllocator(uint maxItems)
    {
        storage.reset(new FreeSlot[maxItems]);
        nextFreeSlot = storage.get();

        FreeSlot* iter = nextFreeSlot;
        for (uint i = 1u; i < maxItems; ++i)
        {
            iter->nextFreeSlot = std::next(iter);
            iter = iter->nextFreeSlot;
        }

        iter->nextFreeSlot = nullptr;
        finalSlot = iter;
    }

    //--------------------------------------------------------//

    template <class... Args>
    Type* allocate(Args&&... args)
    {
        SQASSERT(nextFreeSlot != nullptr, "no more free slots");

        auto ptr = reinterpret_cast<Type*>(nextFreeSlot);
        nextFreeSlot = nextFreeSlot->nextFreeSlot;

        return new (ptr) Type(std::forward<Args>(args)...);
    }

    void deallocate(Type* ptr)
    {
        auto slot = reinterpret_cast<FreeSlot*>(ptr);

        SQASSERT ( slot >= storage.get() && slot <= finalSlot,
                   "pointer outside of allocation range" );

        slot->nextFreeSlot = nextFreeSlot;
        nextFreeSlot = slot;
    }

private: //===================================================//

    struct alignas(Type) FreeSlot
    {
        FreeSlot* nextFreeSlot;
        uchar _padding[Size - 8u];
    };

    unique_ptr<FreeSlot[]> storage;

    FreeSlot* nextFreeSlot;
    FreeSlot* finalSlot;
};

//============================================================================//

/// Bidirectional string to pointer map into a pool.
template <class Type>
class TinyPoolMap final : sq::NonCopyable
{
public: //====================================================//

    struct Item { TinyString key; Type* ptr; };
    using Allocator = PoolAllocator<Type>;

    using iterator = typename std::vector<Item>::iterator;
    using value_type = Item;

    //--------------------------------------------------------//

    TinyPoolMap(Allocator& allocator) : mAllocator(allocator) {}

    //--------------------------------------------------------//

    template <class... Args>
    Type* emplace(TinyString key, Args&&... args)
    {
        SQASSERT(key_exists(key) == false, "");

        Type* ptr = mAllocator.allocate(std::forward(args)...);
        return mItemVector.emplace_back(Item{key, ptr}).ptr;
    }

    //--------------------------------------------------------//

    Type* operator[](TinyString key)
    {
        const auto iter = key_find(key);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->ptr;
    }

    const Type* operator[](TinyString key) const
    {
        const auto iter = key_find(key);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->ptr;
    }

    //--------------------------------------------------------//

    TinyString operator[](const Type* ptr) const
    {
        const auto iter = ptr_find(ptr);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->key;
    }

    //--------------------------------------------------------//

    iterator key_find(TinyString key) const
    {
        auto predicate = [key](const Item& item) { return item.key == key; };
        return algo::find_if(mItemVector, predicate);
    }

    bool key_exists(TinyString key) const
    {
        return key_find(key) != mItemVector.end();
    }

    //--------------------------------------------------------//

    iterator ptr_find(const Type* ptr) const
    {
        auto predicate = [ptr](const Item& item) { return item.ptr == ptr; };
        return algo::find_if(mItemVector, predicate);
    }

    bool ptr_exists(const Type* ptr) const
    {
        return ptr_find(ptr) != mItemVector.end();
    }

private: //===================================================//

    std::vector<Item> mItemVector;

    Allocator& mAllocator;
};

//============================================================================//

} // namespace sq
