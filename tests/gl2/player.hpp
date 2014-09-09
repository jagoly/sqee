#pragma once

#include <SFML/System.hpp>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/resources/texholder.hpp>

#include "objects/models.hpp"
#include "maps/level.hpp"

namespace sqt {

enum class DIR {
    NOPE, N, E, S, W
};

class Player {
public:
    Player();

    sq::TexHolderBasic texHolder;
    sq::Camera* camera;
    Level* level;

    glm::ivec2 gridPos     = {0, 0};
    glm::ivec2 gridPosPrev = {0, 0};
    float zPos = 0.f;
    float zPosPrev = 0.f;
    glm::vec3 pos;
    ushort endTime;
    ushort progress;
    bool moving = false;
    bool moveNext = false;

    std::array<ushort, 4> keys;

    glm::mat4 modelMat;
    Model model;

    void initMove();
    void update_logic(std::array<ushort, 4> _keys);
    bool update_render(double dt, double accum);
};

}
