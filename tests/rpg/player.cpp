#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/cameras.hpp>
#include <sqee/models/animation.hpp>
#include <sqee/globals/resources.hpp>

#include "maps/world.hpp"
#include "player.hpp"

using namespace sqt;

void Player::test_init() {
    layer = "Terrain";

    model.load("Characters/Don",
               "Characters/Don",
               "Characters/Don/Walking");

    sq::Animation* anim = sq::res::animation().add("Characters/Don/Standing");
    anim->create("Characters/Don/Standing");
}

void Player::attempt_move(sq::Direction _moveDir) {
    static sq::Animation* anWalking = sq::res::animation().get("Characters/Don/Walking");

    if (!moving && _moveDir != sq::Direction::Zero) {
        // add collision code here
        gridPrev = gridCrnt;
        moveDir = _moveDir;
        moving = true;
        moveCntr = 0;

        moveVal = {0, 0};
        if      (moveDir == sq::Direction::North) moveVal.y = 1,  rot = glm::radians(180.d);
        else if (moveDir == sq::Direction::East)  moveVal.x = 1,  rot = glm::radians(90.d);
        else if (moveDir == sq::Direction::South) moveVal.y = -1, rot = glm::radians(0.d);
        else if (moveDir == sq::Direction::West)  moveVal.x = -1, rot = glm::radians(-90.d);

        gridCrnt += moveVal;
        camOffsTrgt = glm::vec2(moveVal) * 0.65f;

        world->set_active_tile(gridCrnt);

        if (stopped) {
            model.skeleton.transition(anWalking, 1, 4);
            stopped = false;
        }
    }
}

void Player::tick() {
    static sq::Animation* anStanding = sq::res::animation().get("Characters/Don/Standing");

    model.skeleton.tick();

    posCrnt = posNext;
    camOffsCrnt = camOffsNext;

    if (moving) {
        int prgrs4 = ++moveCntr / 2;
        posNext.z = world->get_maxZ4(layer, (gridPrev.x*4+1) + prgrs4*moveVal.x,
                                            (gridPrev.y*4+1) + prgrs4*moveVal.y);
        if (moveCntr % 2) {
            posNext.z += world->get_maxZ4(layer, (gridPrev.x*4+1) + (prgrs4+1)*moveVal.x,
                                                 (gridPrev.y*4+1) + (prgrs4+1)*moveVal.y);
            posNext.z /= 2.f;
        }

        posNext = glm::vec3(glm::mix(glm::vec2(gridPrev), glm::vec2(gridCrnt),
                                     moveCntr / 8.f), posNext.z);

        camOffsNext = glm::mix(camOffsNext, camOffsTrgt, moveCntr / 8.f);

        if (moveCntr == 8) moving = false;
    } else if (!stopped) {
        model.skeleton.transition(anStanding, 0, 4);
        stopped = true;
    }
}

void Player::calc(double _accum) {
    model.skeleton.calc(_accum);

    const double dt = 1.0 / 24.0;
    glm::vec3 pos = glm::mix(posCrnt, posNext, _accum / dt);
    pos.x += 0.5f; pos.y += 0.5f;

    camera->pos.x = pos.x;
    camera->pos.y = pos.y - 4.5f;
    camera->pos.z = pos.z + 9.5f;

    camera->pos += glm::vec3(glm::mix(camOffsCrnt, camOffsNext, _accum / dt), 0);
    camera->update();

    model.modelMat = glm::translate(glm::mat4(), pos);
    model.modelMat = glm::rotate(model.modelMat, rot, {0, 0, 1});
}
