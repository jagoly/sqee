#pragma once

#include <SFML/System.hpp>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/resources/resholder.hpp>

#include "maps/level.hpp"

namespace sqt {

class Player {
public:
    Player();

    sq::TexHolder texHolder;
    sq::Camera* camera;
    Level* level;

    bool moving = false;
    bool moveNext = false;

    glm::mat4 modelMat;

    void update_logic(std::array<ushort, 4> _keys);
    void update_render(double dt, double accum);

private:
    ushort progress, endTime;
    std::array<ushort, 4> keys;

    glm::ivec2 gridCur = {0, 0};
    glm::ivec2 gridPre = {0, 0};
    float zCur = 0.f;
    float zPre = 0.f;
    std::string layer;
    glm::vec3 pos;

    void initMove();
};

}
