#pragma once

#include <utility>

namespace sq {

template <class Type>
struct Swapper
{
    void swap() { std::swap(front, back); }

    Type front, back;
};

} // namespace sq
