#pragma once

// based on https://github.com/gnzlbg/static_vector/blob/master/include/experimental/fixed_capacity_vector

#include <sqee/debug/Assert.hpp>

#include <cstdint>    // fixed-width integer types
#include <algorithm>  // equal
#include <functional> // less, equal_to
#include <iterator>   // reverse_iterator, iterator traits
#include <stdexcept>  // length_error

namespace sq {

//============================================================================//

/// Dynamically-resizable fixed-capacity vector.
template <class Type, size_t Capacity>
struct StaticVector final
{
public: //====================================================//

    static_assert(std::is_nothrow_destructible_v<Type>, "make sure Type is defined in this context");
    static_assert(Capacity != 0, "Capacity must be non-zero.");

    using value_type       = Type;
    using difference_type  = ptrdiff_t;
    using reference        = Type&;
    using const_reference  = const Type&;
    using pointer          = Type*;
    using const_pointer    = const Type*;
    using iterator         = Type*;
    using const_iterator   = const Type*;

    // 255 because the past the end index must also be valid
    using size_type = std::conditional_t<Capacity < 255u, uint8_t, size_t>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //--------------------------------------------------------//

    Type* data() noexcept { return reinterpret_cast<Type*>(mData); }

    const Type* data() const noexcept { return reinterpret_cast<const Type*>(mData); }

    size_type size() const noexcept { return mSize; }

    static size_type capacity() noexcept { return Capacity; }

    bool empty() const noexcept { return size() == 0u; }

    bool full() const noexcept { return size() == Capacity; }

    //--------------------------------------------------------//

    iterator begin() noexcept { return data(); }

    const_iterator begin() const noexcept { return data(); }

    iterator end() noexcept { return data() + size(); }

    const_iterator end() const noexcept { return data() + size(); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const noexcept { return begin(); }

    const_iterator cend() const noexcept { return end(); }

    //--------------------------------------------------------//

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        SQASSERT(!full(), "vector is full");
        if constexpr (std::is_trivially_destructible_v<Type>)
            *end() = Type(std::forward<Args>(args)...);
        else new (end()) Type(std::forward<Args>(args)...);
        ++mSize;
    }

    template <class Other>
    void push_back(Other&& value)
    {
        static_assert(std::is_convertible_v<Other&&, Type&>);
        emplace_back(std::forward<Other>(value));
    }

    void pop_back()
    {
        SQASSERT(!empty(), "vector is empty");
        std::destroy_at(end()-1);
        --mSize;
    }

    template <class Other>
    iterator insert(iterator iter, Other&& value)
    {
        SQASSERT(!full(), "vector is full");
        SQASSERT(iter >= begin() && iter <= end(), "iter out of range");
        std::move_backward(iter, end(), end()+1);
        new (iter) Type(std::forward<Other>(value));
        ++mSize;
        return iter;
    }

    iterator erase(iterator iter)
    {
        SQASSERT(iter >= begin() && iter < end(), "iter out of range");
        std::move(iter+1, end(), iter);
        pop_back();
        return iter;
    }

    iterator erase(iterator first, iterator last)
    {
        SQASSERT(first >= begin() && last < end() && first <= last, "invalid range");
        auto newEnd = std::move(last, end(), first);
        std::destroy(newEnd, end());
        mSize = std::distance(begin(), newEnd);
        return first;
    }

    void clear() noexcept
    {
        std::destroy(begin(), end());
        mSize = 0u;
    }

    //--------------------------------------------------------//

    Type& operator[](size_type pos) noexcept
    {
        return data()[pos];
    }

    const Type& operator[](size_type pos) const noexcept
    {
        return data()[pos];
    }

    Type& at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("StaticVector::at");

        return data()[pos];
    }

    const Type& at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("StaticVector::at");

        return data()[pos];
    }

    Type& front() noexcept
    {
        SQASSERT(!empty(), "calling front on an empty vector");
        return data()[0];
    }

    const Type& front() const noexcept
    {
        SQASSERT(!empty(), "calling front on an empty vector");
        return data()[0];
    }

    Type& back() noexcept
    {

        SQASSERT(!empty(), "calling back on an empty vector");
        return data()[size() - 1u];
    }

    const Type& back() const noexcept
    {
        SQASSERT(!empty(), "calling back on an empty vector");
        return data()[size() - 1u];
    }

    //--------------------------------------------------------//

    StaticVector() = default;

    StaticVector(const StaticVector& other)
    {
        static_assert(std::is_copy_constructible_v<Type>);
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(other[i]);
    }

    StaticVector(StaticVector&& other) noexcept
    {
        static_assert(std::is_nothrow_move_constructible_v<Type>);
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(std::move(other[i]));
    }

    template <class Other>
    StaticVector(std::initializer_list<Other> il)
    {
        static_assert(std::is_convertible_v<Other, Type>);
        SQASSERT(il.size() <= Capacity, "init list exceeds capacity");
        for (const Other& elem : il)
            emplace_back(elem);
    }

    //--------------------------------------------------------//

    StaticVector& operator=(const StaticVector& other)
    {
        static_assert(std::is_copy_constructible_v<Type>);
        clear();
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(other[i]);
        return *this;
    }

    StaticVector& operator=(StaticVector&& other) noexcept
    {
        static_assert(std::is_nothrow_move_constructible_v<Type>);
        clear();
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(std::move(other[i]));
        return *this;
    }

    //--------------------------------------------------------//

    bool operator==(const StaticVector& other) const
    {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    bool operator!=(const StaticVector& other) const
    {
        return !std::equal(begin(), end(), other.begin(), other.end());
    }

private: //===================================================//

    std::aligned_storage_t<sizeof(Type), alignof(Type)> mData[Capacity] {};

    size_type mSize = 0u;
};

//============================================================================//

} // namespace sq
