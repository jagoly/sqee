// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

namespace sq {

//============================================================================//

namespace detail {

template <class Type> using alignment_size_t =
    std::conditional_t<alignof(Type) <= alignof(uint8_t), uint8_t,
    std::conditional_t<alignof(Type) <= alignof(uint16_t), uint16_t,
    std::conditional_t<alignof(Type) <= alignof(uint32_t), uint32_t,
    size_t>>>;

template <size_t Capacity> using capacity_size_t =
    std::conditional_t<Capacity <= std::numeric_limits<uint8_t>::max(), uint8_t,
    std::conditional_t<Capacity <= std::numeric_limits<uint16_t>::max(), uint16_t,
    std::conditional_t<Capacity <= std::numeric_limits<uint32_t>::max(), uint32_t,
    size_t>>>;

template <class Type, size_t Capacity>
using stackvector_size_t = std::conditional_t <
    sizeof(alignment_size_t<Type>) >= sizeof(capacity_size_t<Capacity>),
    alignment_size_t<Type>, capacity_size_t<Capacity> >;

} // namespace detail

//============================================================================//

/// Dynamically-resizable fixed-capacity vector.
template <class Type, size_t Capacity>
struct StackVector final
{
public: //====================================================//

    static_assert(Capacity != 0u, "capacity must be non-zero");

    using value_type      = Type;
    using difference_type = ptrdiff_t;
    using reference       = Type&;
    using const_reference = const Type&;
    using pointer         = Type*;
    using const_pointer   = const Type*;
    using iterator        = Type*;
    using const_iterator  = const Type*;

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = detail::stackvector_size_t<Type, Capacity>;

    static size_type capacity() { return Capacity; }

    //--------------------------------------------------------//

    Type* data() { return reinterpret_cast<Type*>(mData); }

    const Type* data() const { return reinterpret_cast<const Type*>(mData); }

    size_type size() const { return mSize; }

    bool empty() const { return mSize == 0u; }

    bool full() const { return mSize == Capacity; }

    //--------------------------------------------------------//

    iterator begin() { return data(); }
    const_iterator begin() const { return data(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return data() + size(); }
    const_iterator end() const { return data() + size(); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return rbegin(); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return rend(); }

    //--------------------------------------------------------//

    template <class... Args>
    Type& emplace_back(Args&&... args)
    {
        assert(!full());
        Type& result = *(new (end()) Type(std::forward<Args>(args)...));
        ++mSize;
        return result;
    }

    void push_back(const Type& value)
    {
        emplace_back(value);
    }

    void push_back(Type&& value)
    {
        emplace_back(std::move(value));
    }

    //--------------------------------------------------------//

    template <class... Args>
    iterator emplace(iterator iter, Args&&... args)
    {
        assert(!full() && iter >= begin() && iter <= end());
        std::move_backward(iter, end(), end()+1);
        new (iter) Type(std::forward<Args>(args)...);
        ++mSize;
        return iter;
    }

    iterator insert(iterator iter, const Type& value)
    {
        return emplace(iter, value);
    }

    iterator insert(iterator iter, Type&& value)
    {
        return emplace(iter, std::move(value));
    }

    //--------------------------------------------------------//

    void pop_back()
    {
        assert(!empty());
        std::destroy_at(end()-1);
        --mSize;
    }

    iterator erase(iterator iter)
    {
        assert(!empty() && iter >= begin() && iter < end());
        std::move(iter+1, end(), iter);
        std::destroy_at(end()-1);
        --mSize;
        return iter;
    }

    iterator erase(iterator first, iterator last)
    {
        assert(first <= last && first >= begin() && last <= end());
        const auto newEnd = std::move(last, end(), first);
        std::destroy(newEnd, end());
        mSize = size_type(newEnd - begin());
        return first;
    }

    //--------------------------------------------------------//

    Type& operator[](size_type pos) { assert(pos < size()); return data()[pos]; }
    const Type& operator[](size_type pos) const { assert(pos < size()); return data()[pos]; }

    Type& front() { assert(!empty()); return data()[0]; }
    const Type& front() const { assert(!empty()); return data()[0]; }

    Type& back() { assert(!empty()); return data()[size()-1]; }
    const Type& back() const { assert(!empty()); return data()[size()-1]; }

    //--------------------------------------------------------//

    StackVector() noexcept = default;

    StackVector(const StackVector& other)
    {
        std::uninitialized_copy_n(other.data(), other.size(), data());
        mSize = other.size();
    }

    StackVector(StackVector&& other)
    {
        std::uninitialized_move_n(other.data(), other.size(), data());
        mSize = other.mSize; other.mSize = 0u;
    }

    StackVector(std::initializer_list<Type> il)
    {
        assert(il.size() <= capacity());
        std::uninitialized_copy_n(il.begin(), il.size(), data());
        mSize = size_type(il.size());
    }

    template <class Iter>
    StackVector(Iter first, Iter last)
    {
        // todo: specialise for contiguous iters
        for (auto iter = first; iter != last; ++iter)
            emplace_back(*iter);
    }

    //--------------------------------------------------------//

    ~StackVector()
    {
        std::destroy_n(data(), size());
    }

    void clear()
    {
        std::destroy_n(data(), size());
        mSize = 0u;
    }

    //--------------------------------------------------------//

    StackVector& operator=(const StackVector& other)
    {
        std::destroy_n(data(), size());
        std::uninitialized_copy_n(other.data(), other.size(), data());
        mSize = other.mSize;
        return *this;
    }

    StackVector& operator=(StackVector&& other)
    {
        std::destroy_n(data(), size());
        std::uninitialized_move_n(other.data(), other.size(), data());
        mSize = other.mSize; other.mSize = 0u;
        return *this;
    }

    //--------------------------------------------------------//

    bool operator==(const StackVector& other) const
    {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    bool operator!=(const StackVector& other) const
    {
        return !std::equal(begin(), end(), other.begin(), other.end());
    }

private: //===================================================//

    size_type mSize = 0u;

    std::aligned_storage_t<sizeof(Type), alignof(Type)> mData[Capacity];
};

//============================================================================//

namespace algo {

template <class Type, size_t Capacity, class Value>
inline auto erase(StackVector<Type, Capacity>& container, const Value& value)
{
    const auto iter = std::remove(container.begin(), container.end(), value);
    const auto distance = std::distance(iter, container.end());
    container.erase(iter, container.end());
    return distance;
}

template <class Type, size_t Capacity, class Predicate>
inline auto erase_if(StackVector<Type, Capacity>& container, Predicate pred)
{
    const auto iter = std::remove_if(container.begin(), container.end(), pred);
    const auto distance = std::distance(iter, container.end());
    container.erase(iter, container.end());
    return distance;
}

} // namespace algo

//============================================================================//

} // namespace sq
