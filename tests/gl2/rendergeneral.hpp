#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/extra/gl.hpp>

namespace sqt {

class Camera {
public:
    Camera();

    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    float pos[3] = {0.f, 0.f, 2.f};
    float yaw = 0.f;
};

class Light {
public:
    glm::vec3 worldPos = {0.f, 100.f, 10.f};
    glm::vec3 lightSpec = {1.f, 1.f, 1.f};
    glm::vec3 lightDiff = {0.7f, 0.7f, 0.7f};
    glm::vec3 lightAmbi = {0.2f, 0.2f, 0.2f};
};

}
