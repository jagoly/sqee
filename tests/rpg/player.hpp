#pragma once

#include <SFML/System.hpp>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/camera.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/misc/containers.hpp>

#include "maps/level.hpp"
#include "entities/advmodel.hpp"

namespace sqt {

class Player {
public:
    void set_holders(AdvMeshHolder* _advMeshH, sq::TexHolder* texHolder);

    sq::Camera* camera;
    Level* level;

    AdvMesh* mesh;
    AdvSkin* skin;

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

    AdvMeshHolder* advMeshH;
    sq::TexHolder* texH;

    void initMove();
};

}
