#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/extra/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 pos = {0.f, 2.f, 8.f};
    glm::vec3 lightDiff = {0.8f, 0.8f, 0.8f};
    glm::vec3 lightAmbi = {0.1f, 0.1f, 0.1f};
};

}
