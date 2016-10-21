#pragma once

#include <array>

#include <sqee/setup.hpp>

// unused
// untested
// unfished
// unicycle

namespace sq {

template<class T, size_t N>
class FixedVector
{
public:
    using ArrayType = std::array<T, N>;

    using iterator         = typename ArrayType::iterator;
    using reverse_iterator = typename ArrayType::reverse_iterator;
    using const_iterator         = typename ArrayType::const_iterator;
    using const_reverse_iterator = typename ArrayType::const_reverse_iterator;

//    using size_type       = std::size_t;
//    using difference_type = std::ptrdiff_t;

    size_t size() const { return mCount; }
    bool empty() const { return mCount == 0u; }

    iterator begin() { return mStore.data(); }
    iterator end()   { return mStore.data() + mCount; }

    const_iterator begin() const { return mStore.data(); }
    const_iterator end()   const { return mStore.data() + mCount; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend()   { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend()   const { return const_reverse_iterator(begin()); }

    T&       front()       { return mStore[0]; }
    const T& front() const { return mStore[0]; }

    T&       back()       { return mStore[mCount - 1]; }
    const T& back() const { return mStore[mCount - 1]; }

    T&       operator[](size_t n)       { return mStore[n]; }
    const T& operator[](size_t n) const { return mStore[n]; }

    T&       at(size_t n)       { check(n); return mStore[n]; }
    const T& at(size_t n) const { check(n); return mStore[n]; }

    T*       data()       { return mStore.data(); }
    const T* data() const { return mStore.data(); }

private:
    std::array<T, N> mStore; size_t mCount = 0u;

    void check(size_t n) { if (n >= mCount) throw std::out_of_range(""); }
};

}
