#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/extra/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 pos = {-256.f, -448.f, 1024.f};
    //glm::vec3 pos = {3.5f, 8.f, 5.f};
    glm::vec3 lightDiff = {0.7f, 0.7f, 0.7f};
    glm::vec3 lightAmbi = {0.1f, 0.1f, 0.1f};
};

}
