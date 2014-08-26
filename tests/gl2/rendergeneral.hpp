#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/gl/gl.hpp>

namespace sqt {

class SkyLight {
public:
    glm::vec3 dir = {-0.2f, -0.1f, -0.7f};
    //glm::vec3 dir = {0.f, 0.f, -1.f};
    glm::vec3 diff = {0.8f, 0.8f, 0.8f};
    glm::vec3 ambi = {0.4f, 0.4f, 0.4f};
    glm::vec3 spec = {1.f, 1.f, 1.f};
};

}
