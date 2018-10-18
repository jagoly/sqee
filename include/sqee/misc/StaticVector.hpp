#pragma once

// based on https://github.com/gnzlbg/static_vector/blob/master/include/experimental/fixed_capacity_vector

#include <sqee/debug/Assert.hpp>

#include <cstddef>      // for size_t
#include <cstdint>      // for fixed-width integer types
#include <functional>   // for less and equal_to
#include <iterator>     // for reverse_iterator and iterator traits
#include <stdexcept>    // for length_error

namespace sq {

//============================================================================//

/// Dynamically-resizable fixed-capacity vector.
template <class T, size_t Capacity>
struct StaticVector final
{
public: //====================================================//

    static_assert(std::is_nothrow_destructible_v<T>, "T must be nothrow destructible");
    static_assert(Capacity != 0, "Capacity must be non-zero.");

    using value_type       = T;
    using difference_type  = ptrdiff_t;
    using reference        = T&;
    using const_reference  = const T&;
    using pointer          = T*;
    using const_pointer    = const T*;
    using iterator         = T*;
    using const_iterator   = const T*;

    // 255 because the past the end index must also be valid
    using size_type = std::conditional_t<Capacity < 255u, uint8_t, size_t>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private: //===================================================//

    alignas(T) std::byte mData[sizeof(T) * Capacity] {};

    size_type mSize = 0u;

public: //====================================================//

    constexpr T* data() noexcept { return reinterpret_cast<T*>(mData); }

    constexpr const T* data() const noexcept { return reinterpret_cast<const T*>(mData); }

    constexpr size_type size() const noexcept { return mSize; }

    static constexpr size_type capacity() noexcept { return Capacity; }

    constexpr bool empty() const noexcept { return size() == 0u; }

    constexpr bool full() const noexcept { return size() == Capacity; }

    //--------------------------------------------------------//

    constexpr iterator begin() noexcept { return data();
                                        }
    constexpr const_iterator begin() const noexcept { return data(); }

    constexpr iterator end() noexcept { return data() + size(); }

    constexpr const_iterator end() const noexcept { return data() + size(); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    constexpr const_iterator cbegin() const noexcept { return begin(); }

    constexpr const_iterator cend() const noexcept { return end(); }

    //--------------------------------------------------------//

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        SQASSERT(!full(), "vector is full");
        new (end()) T(std::forward<Args>(args)...);
        ++mSize;
    }

    template <class U>
    constexpr void push_back(U&& value)
    {
        static_assert(std::is_convertible_v<U&&, T&>);
        emplace_back(std::forward<U>(value));
    }

    void pop_back()
    {
        SQASSERT(!empty(), "vector is empty");
        std::prev(end())->~T();
        --mSize;
    }

    constexpr void clear() noexcept
    {
        while (size() != 0u)
            pop_back();
    }

    //--------------------------------------------------------//

    constexpr T& operator[](size_type pos) noexcept
    {
        return data()[pos];
    }

    constexpr const T& operator[](size_type pos) const noexcept
    {
        return data()[pos];
    }

    constexpr T& at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("StaticVector::at");

        return data()[pos];
    }

    constexpr const T& at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("StaticVector::at");

        return data()[pos];
    }

    constexpr T& front() noexcept
    {
        SQASSERT(!empty(), "calling front on an empty vector");
        return data()[0];
    }

    constexpr const T& front() const noexcept
    {
        SQASSERT(!empty(), "calling front on an empty vector");
        return data()[0];
    }

    constexpr T& back() noexcept
    {

        SQASSERT(!empty(), "calling back on an empty vector");
        return data()(size() - 1u);
    }

    constexpr const T& back() const noexcept
    {
        SQASSERT(!empty(), "calling back on an empty vector");
        return data()(size() - 1u);
    }

    //--------------------------------------------------------//

    constexpr StaticVector() = default;

    constexpr StaticVector(const StaticVector& other)
    {
        static_assert(std::is_copy_constructible_v<T>);
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(other[i]);
    }

    constexpr StaticVector(StaticVector&& other)
    {
        static_assert(std::is_move_constructible_v<T>);
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(std::move(other[i]));
    }

    template <class U>
    constexpr StaticVector(std::initializer_list<U> il)
    {
        static_assert(std::is_convertible_v<U, T>);
        SQASSERT(il.size() <= Capacity, "init list exceeds capacity");
        for (size_t i = 0; i < il.size(); ++i)
            emplace_back(index(il, i));
    }

    //--------------------------------------------------------//

    constexpr StaticVector& operator=(const StaticVector& other)
    {
        static_assert(std::is_copy_constructible_v<T>);
        clear();
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(other[i]);
        return *this;
    }

    constexpr StaticVector& operator=(StaticVector&& other)
    {
        static_assert(std::is_move_constructible_v<T>);
        clear();
        for (size_type i = 0u; i < other.size(); ++i)
            emplace_back(std::move(other[i]));
        return *this;
    }
};

//============================================================================//

} // namespace sq
