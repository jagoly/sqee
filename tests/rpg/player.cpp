#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>

#include <sqee/app/logging.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/gl/cameras.hpp>
#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/models/animation.hpp>

#include "maps/world.hpp"
#include "player.hpp"

using namespace sqt;

void Player::test_init() {
    layer = "Terrain";

    model.load("Characters/Don", "Characters/Don", "Characters/Don/Walking");

    sq::Animation* anim = sq::res::animation().add("Characters/Don/Standing");
    anim->create("Characters/Don/Standing");

    lookNext = {0,1,0};
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
        if      (moveDir == sq::Direction::North) moveVal.y = 1,  rot = glm::radians(180.f);
        else if (moveDir == sq::Direction::East)  moveVal.x = 1,  rot = glm::radians(90.f);
        else if (moveDir == sq::Direction::South) moveVal.y = -1, rot = glm::radians(0.f);
        else if (moveDir == sq::Direction::West)  moveVal.x = -1, rot = glm::radians(-90.f);

        gridCrnt += moveVal;

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

    /////
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) zCam += 0.05f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) zCam -= 0.05f;
    /////

    if (moving) {
        int prgrs4 = ++moveCntr / 2;
        posNext.z = world->get_maxZ4(layer, (gridPrev.x*4+1) + prgrs4*moveVal.x,
                                            (gridPrev.y*4+1) + prgrs4*moveVal.y);
        if (moveCntr % 2) {
            posNext.z += world->get_maxZ4(layer, (gridPrev.x*4+1) + (prgrs4+1)*moveVal.x,
                                                 (gridPrev.y*4+1) + (prgrs4+1)*moveVal.y);
            posNext.z /= 2.f;
        }

        posNext = vec3(glm::mix(vec2(gridPrev), vec2(gridCrnt), moveCntr / 8.f), posNext.z);

        if (moveCntr == 8) moving = false;
    } else if (!stopped) {
        model.skeleton.transition(anStanding, 0, 4);
        stopped = true;
    }

    if (settings->crnt<bool>("mouseFocus")) {
        sf::Vector2i mPos = sf::Mouse::getPosition();
        vec2 mMove = {400.f-mPos.x, 300.f-mPos.y};

        lookCrnt = lookNext;
        float nextZ = glm::clamp(lookCrnt.z + mMove.y/400.f, -0.99f, 0.99f);
        vec3 nextXY = glm::rotateZ(vec3(lookCrnt.x, lookCrnt.y, 0.f), mMove.x/200.f);
        lookNext = vec3(nextXY.x, nextXY.y, nextZ);

        sf::Mouse::setPosition({400, 300});
    }
}

void Player::calc(double _accum) {
    model.skeleton.calc(_accum);

    const double dt = 1.0 / 24.0;
    vec3 pos = glm::mix(posCrnt, posNext, _accum / dt);
    pos.x += 0.5f; pos.y += 0.5f;

    camera->pos.x = pos.x;
    camera->pos.y = pos.y;// - 4.5f;
    camera->pos.z = pos.z + zCam;
    //if (moveDir == sq::Direction::North) camera->dir = {0.f, 1.f, 0.f};
    //if (moveDir == sq::Direction::East)  camera->dir = {1.f, 0.f, 0.f};
    //if (moveDir == sq::Direction::South) camera->dir = {0.f, -1.f, 0.f};
    //if (moveDir == sq::Direction::West)  camera->dir = {-1.f, 0.f, 0.f};

    if (settings->crnt<bool>("mouseFocus"))
    camera->dir = glm::normalize(glm::mix(lookCrnt, lookNext, _accum / dt));

    camera->update();
    camera->recalc_frustums();

    model.modelMat = glm::translate(mat4(), pos);
    model.modelMat = glm::rotate(model.modelMat, rot, {0, 0, 1});
}
