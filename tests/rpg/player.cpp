#include <iostream>

#include "player.hpp"

using namespace sqt;

void Player::test_init() {
    layer = "Surface";

    model.load("Characters/Don",
               "res/models/skins/Characters/Don",
               "Characters/Don/Walking");

    sq::Animation* anim = resBank().animH.add("Characters/Don/Standing");
    anim->load("Characters/Don/Standing");

}

void Player::tick(char _moveDir, wld::World& _world) {
    static sq::Animation* anWalking = resBank().animH.get("Characters/Don/Walking");
    static sq::Animation* anStanding = resBank().animH.get("Characters/Don/Standing");

    model.skeleton.tick();

    static uint prgrs8;
    static glm::uvec2 gridPre;
    static int xDir = 0, yDir = 0;
    static int xSub = 0, ySub = 0;

    static bool stopped = true;

    posCrnt = posNext;
    if (!moving && _moveDir != -1) {
        // add collision code here
        gridPre = gridPos;
        moving = true;
        prgrs8 = 0;
        if      (_moveDir == 0) { gridPos.y += 1; rot = 3.1416f; }
        else if (_moveDir == 1) { gridPos.x += 1; rot = 1.5708f; }
        else if (_moveDir == 2) { gridPos.y -= 1; rot = 0.f; }
        else if (_moveDir == 3) { gridPos.x -= 1; rot = -1.5708f; }
        else throw; // Invalid dir

        xDir = gridPos.x - gridPre.x;
        yDir = gridPos.y - gridPre.y;
        xSub = gridPre.x * 4;
        ySub = gridPre.y * 4;

        _world.set_player_pos(gridPos);

        if (stopped) {
            model.skeleton.transition(anWalking, 1, 4);
            stopped = false;
        }
    }

    if (moving) {
        int prgrs4 = ++prgrs8 / 2;

        //posNext.z = _level->get_max16_z(xSub + prgrs4 * xDir, ySub + prgrs4 * yDir, layer);
        //if (prgrs8 % 2) {
        //    posNext.z += _level->get_max16_z(xSub + (prgrs4+1) * xDir, ySub + (prgrs4+1) * yDir, layer);
        //    posNext.z /= 2.f;
        //}

        posNext.x = glm::mix(float(gridPre.x), float(gridPos.x), float(prgrs8) / 8.f);
        posNext.y = glm::mix(float(gridPre.y), float(gridPos.y), float(prgrs8) / 8.f);

        if (prgrs8 == 8) moving = false;
    } else if (!stopped) {
        model.skeleton.transition(anStanding, 0, 4);
        stopped = true;
    }
}

void Player::calc(double _accum, sq::Camera& _camera) {
    model.skeleton.calc(_accum);

    const double dt = 1.d / 24.d;
    glm::vec3 pos = glm::mix(posCrnt, posNext, _accum / dt);
    pos.x += 0.5f; pos.y += 0.5f;

    _camera.pos.x = pos.x;
    _camera.pos.y = pos.y - 3.f;
    _camera.pos.z = pos.z + 8.f;
    _camera.update_projMat();
    _camera.update_viewMat();
    _camera.update_ubo();

    model.modelMat = glm::translate(sq::iMat4, pos);
    model.modelMat = glm::rotate(model.modelMat, rot, {0, 0, 1});
}
