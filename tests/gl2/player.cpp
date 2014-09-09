#include "player.hpp"

using namespace sqt;

Player::Player() {
    model.load_from_dir("player", "player", &texHolder);
    zPos = 2;
}

void Player::initMove() {
    gridPosPrev = gridPos;
    zPosPrev = zPos;
    if      (keys[0] == 0) gridPos.y += 1;
    else if (keys[0] == 1) gridPos.x += 1;
    else if (keys[0] == 2) gridPos.y -= 1;
    else if (keys[0] == 3) gridPos.x -= 1;

    zPos = level->get_tile_z(gridPos.x, gridPos.y, 0);

    progress = 0;
    endTime = 16;
    moving = true;
    moveNext = false;
}

void Player::update_logic(std::array<ushort, 4> _keys) {
    keys = _keys;

    progress += 1;
    if (progress == endTime)
        moving = false;
    if (!moving) {
        if (moveNext) initMove();
        else { // FIX ME, DON'T UPDATE CAMERA UNLESS MOVED
            camera->pos.x = gridPos.x + 0.5f;
            camera->pos.y = gridPos.y - 3.f;
            camera->pos.z = zPos + 8.f;
            camera->update_projMat();
            camera->update_viewMat();
            camera->update_projViewMat();
            modelMat = glm::translate(glm::mat4(), glm::vec3(gridPos.x, gridPos.y, zPos));
        }
        return;
    }
}

bool Player::update_render(double dt, double accum) {
    if (!moving) return true;  // FIX ME, DON'T UPDATE UFORMS EVERY FRAME

    glm::vec3 pos1 = glm::vec3(gridPosPrev.x, gridPosPrev.y, zPosPrev) * (1.f - float(progress) / float(endTime))
                   + glm::vec3(gridPos.x, gridPos.y, zPos) * float(progress) / float(endTime);
    glm::vec3 pos2 = glm::vec3(gridPosPrev.x, gridPosPrev.y, zPosPrev) * (1.f - float(progress+1) / float(endTime))
                   + glm::vec3(gridPos.x, gridPos.y, zPos) * float(progress+1) / float(endTime);

    pos = pos1 * float((dt - accum) / dt) + pos2 * float(accum / dt);

    camera->pos.x = pos.x + 0.5f;
    camera->pos.y = pos.y - 3.f;
    camera->pos.z = pos.z + 8.f;
    camera->update_projMat();
    camera->update_viewMat();
    camera->update_projViewMat();
    modelMat = glm::translate(glm::mat4(), pos);

    return true;
}
