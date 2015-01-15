#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <defs.hpp>

namespace sq {

const glm::mat4 iMat4;

template<class T>
bool within_box(const T& _vec, const T& _min, const T& _max) {
    return glm::all(glm::greaterThanEqual(_vec, _min)) && glm::all(glm::lessThan(_vec, _max));
}

}
