#pragma once

#include <array>
#include <utility>

namespace sq {

template <class Type>
struct Swapper
{
    static_assert(sizeof(Type) == sizeof(ptrdiff_t), "");

    //std::array<Type*, 2> items() { return { &handle, &other }; }

    void swap() { std::swap(front, back); }

    Type front, back;
};

} // namespace sq
