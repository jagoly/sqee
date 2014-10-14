#include "player.hpp"

using namespace sqt;

void Player::set_holders(AdvMeshHolder* _advMeshH, sq::TexHolder* _texH) {
    layer = "Terrain";

    advMeshH = _advMeshH;
    texH = _texH;

    std::string mPath = "res/models/advmeshes/characters/don";
    std::string mName = "don";

    if (!(mesh = advMeshH->get(mName))) {
        mesh = advMeshH->add(mName);
        mesh->load(mPath);
    }
}

void Player::initMove() {
    gridPre = gridCur;
    zPre = zCur;
    if      (keys[0] == 0) gridCur.y += 1;
    else if (keys[0] == 1) gridCur.x += 1;
    else if (keys[0] == 2) gridCur.y -= 1;
    else if (keys[0] == 3) gridCur.x -= 1;

    zCur =                level->get_subtile_z(gridCur.x+1, gridCur.y+1, layer);
    zCur = std::max(zCur, level->get_subtile_z(gridCur.x+2, gridCur.y+1, layer));
    zCur = std::max(zCur, level->get_subtile_z(gridCur.x+1, gridCur.y+2, layer));
    zCur = std::max(zCur, level->get_subtile_z(gridCur.x+2, gridCur.y+2, layer));

    progress = 0;
    endTime = 1;
    moving = true;
    moveNext = false;
}

void Player::update_logic(std::array<ushort, 4> _keys) {
    static ushort subTiles = 3;
    progress += 1;
    if (progress == endTime)
        moving = false;

    if (!moving) {
        if (subTiles != 3) {
            subTiles += 1;
            initMove();
            return;
        }
        if (moveNext) {
            keys = _keys;
            layer = level->get_join(gridCur.x / 4, gridCur.y / 4, layer);
            subTiles = 0;
            initMove();
        } else {
            camera->pos.x = gridCur.x/4.f + 0.5f;
            camera->pos.y = gridCur.y/4.f - 3.f;
            camera->pos.z = zCur + 8.f;
            camera->update_projMat();
            camera->update_viewMat();
            camera->update_ubo();
            modelMat = glm::translate(glm::mat4(), glm::vec3(gridCur.x/4.f +0.5f, gridCur.y/4.f +0.5f, zCur));
        }
        return;
    }
}

void Player::update_render(double dt, double accum) {
    if (!moving) return;

    glm::vec3 pos1 = glm::vec3(gridPre.x/4.f, gridPre.y/4.f, zPre) * (1.f - float(progress) / float(endTime))
                   + glm::vec3(gridCur.x/4.f, gridCur.y/4.f, zCur) * float(progress) / float(endTime);
    glm::vec3 pos2 = glm::vec3(gridPre.x/4.f, gridPre.y/4.f, zPre) * (1.f - float(progress+1) / float(endTime))
                   + glm::vec3(gridCur.x/4.f, gridCur.y/4.f, zCur) * float(progress+1) / float(endTime);

    pos = pos1 * float((dt - accum) / dt) + pos2 * float(accum / dt);

    camera->pos.x = pos.x + 0.5f;
    camera->pos.y = pos.y - 3.f;
    camera->pos.z = pos.z + 8.f;
    camera->update_projMat();
    camera->update_viewMat();
    camera->update_ubo();
    modelMat = glm::translate(glm::mat4(), {pos.x+0.5f, pos.y+0.5f, pos.z});
}
