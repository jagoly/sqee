#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/gl/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 dir = {-0.3f, -0.2f, -0.5f};
    //glm::vec3 dir = {0.f, 0.f, -1.f};
    glm::vec3 lightDiff = {0.7f, 0.7f, 0.7f};
    glm::vec3 lightAmbi = {0.3f, 0.3f, 0.3f};
};

}
