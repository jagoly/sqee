#include <iostream>

#include "player.hpp"

using namespace sqt;

void Player::test_init() {
    layer = "Terrain";

    skeleton.load("res/models/anims/characters/don/walking");
    skel = SkeletonAnim::Ptr(new SkeletonAnim(skeleton));
    skel->tick();

    std::string mPath = "res/models/meshes/characters/don";
    std::string mName = "don";

    if (!(mesh = advMeshH.get(mName))) {
        mesh = advMeshH.add(mName);
        mesh->load(mPath);
    }

    std::string sPath = "res/models/skins/characters/don";
    std::string sName = "don";

    if (!(skin = advSkinH.get(sName))) {
        skin = advSkinH.add(sName);
        skin->load(sPath, texH);
    }
}

void Player::tick(char _moveDir, Level::Ptr& _level) {
    skel->tick();

    static uint prgrs8;
    static glm::ivec2 gridPre;
    static int xDir = 0, yDir = 0;
    static int xSub = 0, ySub = 0;

    posCrnt = posNext;
    if (!moving && _moveDir != -1) {
        // add collision code here
        gridPre = gridPos;
        moving = true;
        prgrs8 = 0;
        if      (_moveDir == 0) gridPos.y += 1;
        else if (_moveDir == 1) gridPos.x += 1;
        else if (_moveDir == 2) gridPos.y -= 1;
        else if (_moveDir == 3) gridPos.x -= 1;
        else throw; // Invalid dir

        xDir = gridPos.x - gridPre.x;
        yDir = gridPos.y - gridPre.y;
        xSub = gridPre.x * 4 + 2;
        ySub = gridPre.y * 4 + 2;
    }

    if (moving) {
        int prgrs4 = ++prgrs8 / 2;

        posNext.z = _level->get_max4_z(xSub + prgrs4 * xDir, ySub + prgrs4 * yDir, layer);
        if (prgrs8 % 2) {
            posNext.z += _level->get_max4_z(xSub + (prgrs4+1) * xDir, ySub + (prgrs4+1) * yDir, layer);
            posNext.z /= 2.f;
        }

        posNext.x = glm::mix(float(gridPre.x), float(gridPos.x), float(prgrs8) / 8.f);
        posNext.y = glm::mix(float(gridPre.y), float(gridPos.y), float(prgrs8) / 8.f);

        if (prgrs8 == 8) moving = false;
    }
}

void Player::calc(double _accum, sq::Camera& _camera) {
    skel->calc(_accum);

    const double dt = 1.d / 24.d;
    glm::vec3 pos = glm::mix(posCrnt, posNext, _accum / dt);
    pos.x += 0.5f; pos.y += 0.5f;

    _camera.pos.x = pos.x;
    _camera.pos.y = pos.y - 3.f;
    _camera.pos.z = pos.z + 8.f;
    _camera.update_projMat();
    _camera.update_viewMat();
    _camera.update_ubo();

    modelMat = glm::translate(sq::iMat4, pos);
}
