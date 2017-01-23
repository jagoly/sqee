#pragma once

#include <vector>

#include <sqee/assert.hpp>
#include <sqee/misc/Algorithms.hpp>

namespace sq { namespace dop {

//===========================================================================//

template <class Type>
struct Entry
{
    Entry() : id(-1), data(nullptr) {}
    Entry(int32_t id, Type* ptr) : id(id), data(ptr) {}

    Type*       operator->()       { return data; }
    const Type* operator->() const { return data; }

    const int32_t id; Type* const data;
};

//============================================================================//

class Group : public MoveOnly
{
public:

    //========================================================//

    /// return the size of the group
    uint size() const { return mIds.size(); }

    /// check if the group is empty
    bool empty() const { return mIds.empty(); }

    //========================================================//

    /// pre-allocate memory
    void reserve(uint count);

    /// re-allocate to fit size
    void shrink_to_fit();

    /// clear the group
    void clear();

    //========================================================//

    /// add an id to the group
    void insert(int32_t id);

    /// remove an id from the group
    void erase(int32_t id);

    //========================================================//

    std::vector<int32_t> mIds;
    bool mIsSorted = true;
};

//============================================================================//

template <class Type>
class Table : public MoveOnly
{
public:

    //========================================================//

    using data_type = Type;
    using const_data_type = const Type;

    using entry_type = Entry<Type>;
    using const_entry_type = Entry<const Type>;

    //========================================================//

    /// return the table size
    uint size() const { return mIds.size(); }

    /// check if table is empty
    bool empty() const { return mIds.empty(); }

    //========================================================//

    /// get the most recently added entry
    Entry<Type> back() { return { mIds.back(), &mData.back() }; }

    /// get the most recently added entry
    Entry<const Type> back() const { return { mIds.back(), &mData.back() }; }

    //========================================================//

    /// pre-allocate memory
    void reserve(uint count);

    /// re-allocate to fit size
    void shrink_to_fit();

    /// clear the table
    void clear();

    //========================================================//

    /// add an entry to the table
    void insert(int32_t id, Type data);

    /// remove an entry from the table
    void erase(int32_t id);

    //========================================================//

    /// get data for a known entry
    Type& get(int32_t id);

    /// get data for a known entry
    const Type& get(int32_t id) const;

    //========================================================//

    /// try to get data for an entry
    Type* try_get(int32_t id);

    /// try to get data for an entry
    const Type* try_get(int32_t id) const;

    //========================================================//

    std::vector<int32_t> mIds;
    std::vector<Type> mData;
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
void Table<Type>::insert(int32_t id, Type data)
{
    const auto iter = algo::find(mIds, id);
    SQASSERT(iter == mIds.end(), "id already used");

    if (mIsSorted == true && size() != 0u)
        mIsSorted = mIds.back() < id;

    mIds.push_back(id);
    mData.push_back(std::move(data));
}

//============================================================================//

template <class Type> inline
void Table<Type>::erase(int32_t id)
{
    const auto iter = algo::find(mIds, id);
    SQASSERT(iter != mIds.end(), "id not found");

    mIds.erase(iter);
    mData.erase(mData.begin() + iter - mIds.begin());
}

//============================================================================//

template <class Type> inline
Type& Table<Type>::get(int32_t id)
{
    const auto iter = algo::find(mIds, id);
    SQASSERT(iter != mIds.end(), "id not found");
    return mData[iter - mIds.begin()];
}

template <class Type> inline
const Type& Table<Type>::get(int32_t id) const
{
    const auto iter = algo::find(mIds, id);
    SQASSERT(iter != mIds.end(), "id not found");
    return mData[iter - mIds.begin()];
}

//============================================================================//

template <class Type> inline
Type* Table<Type>::try_get(int32_t id)
{
    const auto iter = algo::find(mIds, id);
    if (iter == mIds.end()) return nullptr;
    return &mData[iter - mIds.begin()];
}

template <class Type> inline
const Type* Table<Type>::try_get(int32_t id) const
{
    const auto iter = algo::find(mIds, id);
    if (iter == mIds.end()) return nullptr;
    return &mData[iter - mIds.begin()];
}

//============================================================================//

}} // namespace sq::dop
