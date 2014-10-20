#pragma once

#include <libsqee/gl/gl.hpp>

namespace sqt {

class Settings {
public:
    int aa = 2;
    int shad = 4;
    int refl = 2;
};

class SkyLight {
public:
    glm::vec3 dir = {-0.1f, -0.25f, -0.65f};
    //glm::vec3 dir = {0.5f, 0.f, -0.5f};
    glm::vec3 diff = {0.8f, 0.8f, 0.8f};
    glm::vec3 ambi = {0.4f, 0.4f, 0.4f};
    glm::vec3 spec = {1.f, 1.f, 1.f};

    glm::mat4 viewMat;
    glm::mat4 projMat;
    glm::mat4 projViewMat;

    void update(glm::vec3 _dir, glm::uvec3 _mapSize);
};

}
