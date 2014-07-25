#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/extra/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 pos = {-128.f, -256.f, 192.f};
    glm::vec3 lightDiff = {0.8f, 0.8f, 0.8f};
    glm::vec3 lightAmbi = {0.1f, 0.1f, 0.1f};
};

}
