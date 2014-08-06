#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <libsqee/gl/gl.hpp>

namespace sqt {

class Player {
public:
    Player();

    int gridPos[3] = {0, 0, 0};
    int gridPosPrev[3] = {0, 0, 0};
    float progress = 0.f;

    glm::vec3 pos = {0.5f, 0.5f, 0.f};

    GLuint vao, tex;

    void update();
};

}
