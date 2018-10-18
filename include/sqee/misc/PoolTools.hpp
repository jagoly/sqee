#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Algorithms.hpp>
#include <sqee/misc/Builtins.hpp>

namespace sq {

//============================================================================//

/// A Simple Pool Allocator.
template <class Type>
class PoolAllocator final : private NonCopyable
{
public: //====================================================//

    static_assert(std::is_trivially_copyable_v<Type>);
    static_assert(std::is_trivially_destructible_v<Type>);

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

    UniquePtr<FreeSlot[]> storage;

    FreeSlot* nextFreeSlot;
    FreeSlot* finalSlot;
};

//============================================================================//

/// Bidirectional map of items in a pool.
template <class Key, class Type>
class TinyPoolMap final : private NonCopyable
{
public: //====================================================//

    struct Item { Key key; Type* ptr; };
    using Allocator = PoolAllocator<Type>;

    using iterator = typename Vector<Item>::iterator;
    using const_iterator = typename Vector<Item>::const_iterator;
    using value_type = Item;

    //--------------------------------------------------------//

    TinyPoolMap(Allocator& allocator) : mAllocator(allocator) {}

    //--------------------------------------------------------//

    iterator begin() { return mItemVector.begin(); }
    iterator end() { return mItemVector.end(); }

    const_iterator cbegin() const { return mItemVector.cbegin(); }
    const_iterator cend() const { return mItemVector.cend(); }

    const_iterator begin() const { return mItemVector.cbegin(); }
    const_iterator end() const { return mItemVector.cend(); }

    //--------------------------------------------------------//

    template <class... Args>
    Type* emplace(Key key, Args&&... args)
    {
        SQASSERT(key_exists(key) == false, "");

        Type* ptr = mAllocator.allocate(std::forward(args)...);
        mItemVector.push_back({ key, ptr });

        return ptr;
    }

    void erase(const Key& key)
    {
        const auto iter = key_find(key);
        SQASSERT(iter != mItemVector.end(), "");
        mAllocator.deallocate(iter->ptr);
        mItemVector.erase(iter);
    }

    void clear()
    {
        for (auto& item : mItemVector)
            mAllocator.deallocate(item.ptr);

        mItemVector.clear();
    }

    //--------------------------------------------------------//

    Type* operator[](const Key& key)
    {
        const auto iter = key_find(key);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->ptr;
    }

    const Type* operator[](const Key& key) const
    {
        const auto iter = key_find(key);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->ptr;
    }

    const Key& operator[](const Type* ptr) const
    {
        const auto iter = ptr_find(ptr);
        SQASSERT(iter != mItemVector.end(), "");
        return iter->key;
    }

    //--------------------------------------------------------//

    iterator key_find(const Key& key)
    {
        auto predicate = [&](const Item& item) { return item.key == key; };
        return algo::find_if(mItemVector, predicate);
    }

    const_iterator key_find(const Key& key) const
    {
        auto predicate = [&](const Item& item) { return item.key == key; };
        return algo::find_if(mItemVector, predicate);
    }

    const_iterator ptr_find(const Type* ptr) const
    {
        auto predicate = [&](const Item& item) { return item.ptr == ptr; };
        return algo::find_if(mItemVector, predicate);
    }

    //--------------------------------------------------------//

    bool key_exists(const Key& key) const
    {
        return key_find(key) != mItemVector.end();
    }

    bool ptr_exists(const Type* ptr) const
    {
        return ptr_find(ptr) != mItemVector.end();
    }

private: //===================================================//

    Vector<Item> mItemVector;

    Allocator& mAllocator;
};

//============================================================================//

} // namespace sq
