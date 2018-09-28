#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Algorithms.hpp>
#include <sqee/misc/Builtins.hpp>

namespace sq::dop {

//============================================================================//

/// Set of unique ids.
class SQEE_API Group : private MoveOnly
{
public: //====================================================//

    /// Get the size of the group.
    uint size() const { return uint(mIds.size()); }

    /// Check if the group is empty.
    bool empty() const { return mIds.empty(); }

    //--------------------------------------------------------//

    /// Allocate storage.
    void reserve(size_t count);

    /// Shrink storage.
    void shrink_to_fit();

    /// Clear the group.
    void clear();

    //--------------------------------------------------------//

    /// Add an ID to the group.
    void insert(int32_t id);

    /// Remove an ID from the group.
    void erase(int32_t id);

    //--------------------------------------------------------//

    Vector<int32_t> mIds;

    bool mIsSorted = true;
};

//===========================================================================//

/// Provides access to an item in a @ref Table.
/// @tparam Type the type of the table data

template <class Type>
struct Entry
{
    Entry() : id(-1), data(nullptr) {}
    Entry(int32_t id, Type* ptr) : id(id), data(ptr) {}

    Type*       operator->()       { return data; }
    const Type* operator->() const { return data; }

    Type&       operator*()       { return *data; }
    const Type& operator*() const { return *data; }

    const int32_t id; Type* const data;
};

//============================================================================//

/// Cache friendly mapping of ids to data.
/// @tparam Type the type of the table data

template <class Type>
class Table : private MoveOnly
{
public: //====================================================//

    using data_type = Type;
    using const_data_type = const Type;

    using entry_type = Entry<Type>;
    using const_entry_type = Entry<const Type>;

    //--------------------------------------------------------//

    /// Get the size of the table.
    uint size() const { return uint(mIds.size()); }

    /// Check if table is empty.
    bool empty() const { return mIds.empty(); }

    //--------------------------------------------------------//

    /// Access the most recently added entry.
    Entry<Type> back() { return { mIds.back(), &mData.back() }; }

    /// Access the most recently added entry.
    Entry<const Type> back() const { return { mIds.back(), &mData.back() }; }

    //--------------------------------------------------------//

    /// Allocate storage.
    void reserve(uint count);

    /// Shrink storage.
    void shrink_to_fit();

    /// Clear the table.
    void clear();

    //--------------------------------------------------------//

    /// Check if an entry exists.
    bool exists(int32_t id) const;

    /// Find the index of an entry.
    uint find(int32_t id) const;

    //--------------------------------------------------------//

    /// Add an entry to the table.
    void insert(int32_t id, Type data);

    //--------------------------------------------------------//

    /// Remove an entry by index.
    void erase_at(uint index);

    /// Try to remove an entry from the table.
    bool try_erase(int32_t id);

    //--------------------------------------------------------//

    /// Remove all entries satisfying a predicate.
    template <class Predicate>
    uint erase_if(const Predicate& predicate);

    //--------------------------------------------------------//

    /// Get data for a known entry.
    Type& get(int32_t id);

    /// Get data for a known entry.
    const Type& get(int32_t id) const;

    //--------------------------------------------------------//

    /// Try to get data for an entry.
    Type* try_get(int32_t id);

    /// Try to get data for an entry.
    const Type* try_get(int32_t id) const;

    //--------------------------------------------------------//

    Vector<int32_t> mIds;
    Vector<Type> mData;

    bool mIsSorted = true;
};

//============================================================================//

template <class Type> inline
void Table<Type>::reserve(uint count)
{
    mIds.reserve(count);
    mData.reserve(count);
}

template <class Type> inline
void Table<Type>::shrink_to_fit()
{
    mIds.shrink_to_fit();
    mData.shrink_to_fit();
}

template <class Type> inline
void Table<Type>::clear()
{
    mIsSorted = true;
    mIds.clear();
    mData.clear();
}

//============================================================================//

template <class Type> inline
bool Table<Type>::exists(int32_t id) const
{
    return algo::exists(mIds, id);
}

template <class Type> inline
uint Table<Type>::find(int32_t id) const
{
    const auto iter = algo::find(mIds, id);
    return uint(std::distance(mIds.begin(), iter));
}

//============================================================================//

template <class Type> inline
void Table<Type>::insert(int32_t id, Type data)
{
    SQASSERT(!exists(id), "id already used");

    if (mIsSorted == true && size() != 0u)
        mIsSorted = mIds.back() < id;

    mIds.push_back(id);
    mData.push_back(std::move(data));
}

//============================================================================//

template <class Type> inline
void Table<Type>::erase_at(uint index)
{
    SQASSERT(index < size(), "index out of range");

    mIds.erase(mIds.begin() + index);
    mData.erase(mData.begin() + index);
}

template <class Type> inline
bool Table<Type>::try_erase(int32_t id)
{
    const uint index = find(id);
    if (index >= size()) return false;
    erase_at(index); return true;
}

//============================================================================//

template <class Type> template <class Predicate> inline
uint Table<Type>::erase_if(const Predicate& predicate)
{
    auto idsIter = mIds.begin();
    auto dataIter = mData.begin();
    uint eraseCount = 0u;

    while (idsIter != mIds.end())
    {
        if (predicate(*idsIter, *dataIter))
        {
            idsIter = mIds.erase(idsIter);
            dataIter = mData.erase(dataIter);
            eraseCount += 1u;
        }
        else { ++idsIter; ++dataIter; }
    }

    return eraseCount;
}

//============================================================================//

template <class Type> inline
Type& Table<Type>::get(int32_t id)
{
    const uint index = find(id);
    SQASSERT(index < size(), "id not found");
    return mData[index];
}

template <class Type> inline
const Type& Table<Type>::get(int32_t id) const
{
    const uint index = find(id);
    SQASSERT(index < size(), "id not found");
    return mData[index];
}

//============================================================================//

template <class Type> inline
Type* Table<Type>::try_get(int32_t id)
{
    const uint index = find(id);
    if (index >= size()) return nullptr;
    return &mData[index];
}

template <class Type> inline
const Type* Table<Type>::try_get(int32_t id) const
{
    const uint index = find(id);
    if (index >= size()) return nullptr;
    return &mData[index];
}

//============================================================================//

} // namespace sq::dop
