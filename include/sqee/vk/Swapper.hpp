#pragma once

#include <utility>

namespace sq {

template <class Type>
struct Swapper
{
    static_assert(sizeof(Type) == sizeof(void*), "");

    void swap() { std::swap(front, back); }

    Type front, back;
};

} // namespace sq
