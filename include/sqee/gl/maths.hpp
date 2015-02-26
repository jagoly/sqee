#pragma once
#include "forward.hpp"

#include <glm/vector_relational.hpp>

namespace sq {

template<class T>
bool within_box(const T& _vec, const T& _min, const T& _max) {
    return glm::all(glm::greaterThanEqual(_vec, _min) + glm::lessThan(_vec, _max));
}

}
