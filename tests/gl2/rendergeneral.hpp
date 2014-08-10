#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/gl/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 dir = {-0.3f, -0.2f, -0.5f};
    glm::vec3 diff = {0.7f, 0.7f, 0.7f};
    glm::vec3 ambi = {0.3f, 0.3f, 0.3f};
    glm::vec3 spec = {1.f, 1.f, 1.f};
};

}
