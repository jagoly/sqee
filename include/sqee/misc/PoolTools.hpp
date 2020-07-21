#pragma once

#include <sqee/debug/Assert.hpp>

#include <algorithm> // lower_bound, upper_bound, equal, sort
#include <memory>    // unique_ptr
#include <utility>   // pair, move, hash

namespace sq {

//============================================================================//

template <class Type> class PoolAllocator;

/// Storage for sq::PoolAllocators.
template <class Type>
class PoolAllocatorStore final
{
public: //====================================================//

    static_assert(std::is_nothrow_destructible_v<Type>, "make sure Type is defined in this context");

    static constexpr size_t Size = sizeof(Type);

    //--------------------------------------------------------//

    PoolAllocatorStore(uint maxItems)
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

    PoolAllocatorStore(const PoolAllocatorStore&) = delete;
    PoolAllocatorStore(PoolAllocatorStore&&) = delete;
    PoolAllocatorStore& operator=(const PoolAllocatorStore&) = delete;
    PoolAllocatorStore& operator=(PoolAllocatorStore&&) = delete;

    //--------------------------------------------------------//

    Type* allocate(size_t n)
    {
        SQASSERT(n == 1u, "invalid allocation item count");
        SQASSERT(nextFreeSlot != nullptr, "no more free slots");

        if (n != 1u || nextFreeSlot == nullptr)
            throw std::bad_alloc();

        auto result = reinterpret_cast<Type*>(nextFreeSlot);

        nextFreeSlot = nextFreeSlot->nextFreeSlot;

        return result;
    }

    void deallocate(Type* ptr, size_t n) noexcept
    {
        SQASSERT(n == 1u, "invalid allocation item count");
        SQASSERT(impl_ptr_in_range(ptr) == true, "pointer out of range");

        auto slot = reinterpret_cast<FreeSlot*>(ptr);

        // todo: this should reduce fragmentation, test that it's actually correct
        if (slot < nextFreeSlot)
        {
            slot->nextFreeSlot = nextFreeSlot;
            nextFreeSlot = slot;
        }
        else
        {
            slot->nextFreeSlot = nextFreeSlot->nextFreeSlot;
            nextFreeSlot->nextFreeSlot = slot;
        }
    }

    template <class... Args>
    void construct(Type* ptr, Args&&... args)
    {
        SQASSERT(impl_ptr_in_range(ptr) == true, "pointer out of range");

        new (ptr) Type(std::forward<Args>(args)...);
    }

    void destroy(Type* ptr) noexcept
    {
        SQASSERT(impl_ptr_in_range(ptr) == true, "pointer out of range");

        ptr->~Type();
    }

    //--------------------------------------------------------//

    PoolAllocator<Type> get() { return { *this }; }

private: //===================================================//

    bool impl_ptr_in_range(Type* ptr) const noexcept
    {
        auto slot = reinterpret_cast<FreeSlot*>(ptr);
        return slot >= storage.get() && slot <= finalSlot;
    }

    struct alignas(Type) FreeSlot
    {
        FreeSlot* nextFreeSlot;
        uint8_t _padding[Size - 8u];
    };

    std::unique_ptr<FreeSlot[]> storage;

    FreeSlot* nextFreeSlot;
    FreeSlot* finalSlot;
};

//============================================================================//

template <class Type>
class PoolAllocator final
{
public: //====================================================//

    using value_type = Type;
    using pointer = Type*;

    PoolAllocator(PoolAllocatorStore<Type>& store) : mStore(&store) {}

    pointer allocate(size_t n) { return mStore->allocate(n); }

    void deallocate(pointer ptr, size_t n) noexcept { mStore->deallocate(ptr, n); }

    template <class... Args>
    void construct(pointer ptr, Args&&... args) { mStore->construct(ptr, std::forward<Args>(args)...); }

    void destroy(pointer ptr) noexcept { mStore->destroy(ptr); }

    bool operator==(const PoolAllocator& other) const { return mStore == other.mStore; }

private: //===================================================//

    PoolAllocatorStore<Type>* mStore;
};

static_assert (std::is_copy_constructible_v<PoolAllocator<float>>);
static_assert (std::is_copy_assignable_v<PoolAllocator<float>>);
static_assert (std::is_nothrow_destructible_v<PoolAllocator<float>>);

//============================================================================//

// PoolMap interface should almost entirely match std::[unordered_]map

//template <class Key, class Value, class Compare = std::less<Key>>
//using PoolMap = std::map<Key, Value, Compare, PoolAllocator<std::pair<const Key, Value>>>;

//============================================================================//

template <class Key, class Type>
struct PoolMapIterator
{
    using value_type = std::pair<const Key, Type>;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::bidirectional_iterator_tag;

    using BaseIter = typename std::vector<value_type*>::iterator;

    explicit PoolMapIterator(BaseIter _base) : base(_base) {}

    value_type& operator*() const { return *(*base); }
    value_type* operator->() const { return *base; }

    bool operator==(PoolMapIterator other) const { return base == other.base; }
    bool operator!=(PoolMapIterator other) const { return base != other.base; }
    bool operator<=(PoolMapIterator other) const { return base <= other.base; }
    bool operator>=(PoolMapIterator other) const { return base >= other.base; }
    bool operator<(PoolMapIterator other) const { return base < other.base; }
    bool operator>(PoolMapIterator other) const { return base > other.base; }

    PoolMapIterator& operator--() { --base; return *this; }
    PoolMapIterator& operator++() { ++base; return *this; }

    BaseIter base;
};

template <class Key, class Type>
struct PoolMapConstIterator
{
    using value_type = std::pair<const Key, Type>;
    using difference_type = ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::bidirectional_iterator_tag;

    using BaseIter = typename std::vector<value_type*>::const_iterator;

    explicit PoolMapConstIterator(BaseIter _base) : base(_base) {}
    PoolMapConstIterator(PoolMapIterator<Key, Type> nonConst) : base(nonConst.base) {}

    const value_type& operator*() const { return *(*base); }
    const value_type* operator->() const { return *base; }

    bool operator==(PoolMapConstIterator other) const { return base == other.base; }
    bool operator!=(PoolMapConstIterator other) const { return base != other.base; }
    bool operator<=(PoolMapConstIterator other) const { return base <= other.base; }
    bool operator>=(PoolMapConstIterator other) const { return base >= other.base; }
    bool operator<(PoolMapConstIterator other) const { return base < other.base; }
    bool operator>(PoolMapConstIterator other) const { return base > other.base; }

    PoolMapConstIterator& operator--() { --base; return *this; }
    PoolMapConstIterator& operator++() { ++base; return *this; }

    BaseIter base;
};

/// Map for items allocated from a pool. It makes use of a simple hash table.
///
/// Pointers to items are never invalidated.
/// iterators may be invalidated whenever an item is inserted or removed.
///
template <class Key, class Type>
class PoolMap final
{
public: //====================================================//

    static_assert(std::is_nothrow_destructible_v<Type>, "make sure Type is defined in this context");

    using value_type = std::pair<const Key, Type>;

    using difference_type  = ptrdiff_t;
    using reference        = value_type&;
    using const_reference  = const value_type&;
    using pointer          = value_type*;
    using const_pointer    = const value_type*;

    using allocator_type = PoolAllocator<value_type>;

    using iterator       = PoolMapIterator<Key, Type>;
    using const_iterator = PoolMapConstIterator<Key, Type>;

    //--------------------------------------------------------//

    class node_type
    {
    public:

        node_type(allocator_type allocator, value_type* ptr) : mAllocator(allocator), mPtr(ptr) {}
        node_type(node_type&& other) noexcept : node_type(other.mAllocator, other.mPtr) { other.mPtr = nullptr; }
        ~node_type() noexcept { if (!empty()) { mAllocator.destroy(mPtr); mAllocator.deallocate(mPtr, 1u); } }

        node_type(const node_type&) = delete;
        node_type& operator=(const node_type&) = delete;

        bool empty() const { return mPtr == nullptr; }
        explicit operator bool() const { return !empty(); }

        allocator_type get_allocator() const { SQASSERT(!empty(), ""); return mAllocator; }
        Key& key() const { SQASSERT(!empty(), ""); return const_cast<Key&>(mPtr->first); }
        Type& mapped() const { SQASSERT(!empty(), ""); return mPtr->second; }

    private:

        allocator_type mAllocator;
        value_type* mPtr;
        friend class PoolMap;
    };

    //--------------------------------------------------------//

    /// Constructor
    PoolMap(allocator_type allocator) : mAllocator(allocator) {}

    /// Copy Constructor
    PoolMap(const PoolMap& other) : PoolMap(other.mAllocator)
    {
        reserve(other.capacity());
        for (const auto& [key, value] : other)
        {
            value_type* ptr = mAllocator.allocate(1u);
            mAllocator.construct(ptr, key, value);
            mItemVector.emplace_back(ptr);
        }
        impl_hash_table_regenerate();
    }

    /// Move Constructor
    PoolMap(PoolMap&& other) noexcept : PoolMap(other.mAllocator)
    {
        mItemVector = std::move(other.mItemVector);
        mHashTable = std::move(other.mHashTable);
    }

    /// Copy Assignment
    PoolMap& operator=(const PoolMap& other)
    {
        SQASSERT(mAllocator == other.mAllocator, "PoolMap copy with different allocators");
        clear();
        reserve(other.capacity());
        for (const auto& [key, value] : other)
        {
            value_type* ptr = mAllocator.allocate(1u);
            mAllocator.construct(ptr, key, value);
            mItemVector.emplace_back(ptr);
        }
        impl_hash_table_regenerate();
        return *this;
    }

    /// Move Assignment
    PoolMap& operator=(PoolMap&& other) noexcept
    {
        SQASSERT(mAllocator == other.mAllocator, "PoolMap move with different allocators");
        clear();
        mItemVector = std::move(other.mItemVector);
        mHashTable = std::move(other.mHashTable);
        return *this;
    }

    /// Destructor
    ~PoolMap() noexcept { clear(); }

    //--------------------------------------------------------//

    size_t capacity() const { return mItemVector.capacity(); }

    size_t size() const { return mItemVector.size(); }

    void reserve(size_t _capacity)
    {
        if (_capacity > mItemVector.capacity())
        {
            mItemVector.reserve(_capacity);
            impl_hash_table_regenerate();
        }
    }

    //--------------------------------------------------------//

    iterator begin() { return iterator(mItemVector.begin()); }
    iterator end() { return iterator(mItemVector.end()); }

    const_iterator cbegin() const { return const_iterator(mItemVector.cbegin()); }
    const_iterator cend() const { return const_iterator(mItemVector.cend()); }

    const_iterator begin() const { return const_iterator(mItemVector.cbegin()); }
    const_iterator end() const { return const_iterator(mItemVector.cend()); }

    //--------------------------------------------------------//

    template <class... Args>
    std::pair<iterator, bool> try_emplace(Key key, Args&&... args)
    {
        if (auto iter = find(key); iter != end())
            return { iter, false };
        value_type* ptr = mAllocator.allocate(1u);
        mAllocator.construct(ptr, std::piecewise_construct, std::forward_as_tuple(key),
                             std::forward_as_tuple(std::forward<Args>(args)...));
        auto iter = mItemVector.emplace(impl_find_insert_pos(key), ptr);
        impl_hash_table_regenerate();
        return { iterator(iter), true };
    }

    iterator erase(iterator iter)
    {
        SQASSERT(iter >= begin() && iter < end(), "iter out of range");
        mAllocator.destroy(&*iter);
        mAllocator.deallocate(&*iter, 1u);
        mItemVector.erase(iter.base);
        impl_hash_table_regenerate();
        return iter;
    }

    void clear()
    {
        for (value_type* ptr : mItemVector)
        {
            mAllocator.destroy(ptr);
            mAllocator.deallocate(ptr, 1u);
        }
        mItemVector.clear();
        impl_hash_table_regenerate();
    }

    //--------------------------------------------------------//

    iterator insert(node_type&& node)
    {
        SQASSERT(node.empty() == false, "cannot insert empty node");
        SQASSERT(find(node.key()) == end(), "key already exists");
        auto iter = mItemVector.emplace(impl_find_insert_pos(node.key()), node.mPtr);
        node.mPtr = nullptr;
        impl_hash_table_regenerate();
        return iterator(iter);
    }

    node_type extract(iterator iter)
    {
        SQASSERT(iter >= begin() && iter < end(), "iter out of range");
        node_type result { mAllocator, &*iter };
        mItemVector.erase(iter.base);
        impl_hash_table_regenerate();
        return result;
    }

    //--------------------------------------------------------//

    Type& at(const Key& key)
    {
        const auto iter = find(key);
        if (iter == end()) throw std::out_of_range("sq::PoolMap");
        return iter->second;
    }

    Type& operator[](const Key& key)
    {
        auto [iter, ok] = try_emplace(key);
        //SQASSERT(ok == false, "key does not exist");
        return iter->second;
    }

    const Type& operator[](const Key& key) const
    {
        const auto iter = find(key);
        SQASSERT(iter != end(), "key does not exist");
        return iter->second;
    }

    //--------------------------------------------------------//

    bool operator==(const PoolMap& other) const
    {
        // this won't compare equal if the two maps are not ordered the same way
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    bool operator!=(const PoolMap& other) const
    {
        return !(*this == other);
    }

    //--------------------------------------------------------//

    iterator find(const Key& key)
    {
        const auto hashIter = impl_find(key);
        if (hashIter == mHashTable.end()) return end();
        return hashIter->second;
    }

    const_iterator find(const Key& key) const
    {
        const auto hashIter = impl_find(key);
        if (hashIter == mHashTable.end()) return end();
        return hashIter->second;
    }

    //--------------------------------------------------------//

    size_t count(const Key& key) const
    {
        return size_t(find(key) != end());
    }

private: //===================================================//

    std::vector<value_type*> mItemVector;

    std::vector<std::pair<size_t, iterator>> mHashTable;

    allocator_type mAllocator;

    //--------------------------------------------------------//

    auto impl_find(const Key& key) const
    {
        const size_t hash = std::hash<Key>()(key);

        const auto compare = [](auto& item, auto& hash) { return item.first < hash; };
        auto hashIter = std::lower_bound(mHashTable.begin(), mHashTable.end(), hash, compare);
        while (hashIter != mHashTable.end() && hashIter->first == hash)
        {
            if (hashIter->second->first == key) return hashIter;
            ++hashIter;
        }

        return mHashTable.end();
    }

    auto impl_find_insert_pos(const Key& key)
    {
        const auto compare = [](auto& item, auto& key) { return item->first < key; };
        return std::lower_bound(mItemVector.begin(), mItemVector.end(), key, compare);
    }

    void impl_hash_table_regenerate()
    {
        mHashTable.clear();
        mHashTable.reserve(mItemVector.capacity());

        for (auto iter = begin(); iter != end(); ++iter)
            mHashTable.emplace_back(std::hash<Key>()(iter->first), iter);

        const auto compare = [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; };
        std::sort(mHashTable.begin(), mHashTable.end(), compare);
    }
};

//============================================================================//

} // namespace sq
