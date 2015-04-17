#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/application.hpp>
#include <sqee/models/modelskelly.hpp>

#include "render/camera.hpp"
#include "player.hpp"

using namespace sqt;

void Player::test_init() {
    model.reset(new sq::ModelSkelly());
    model->skel.tickRate = 24;
    if (!(model->mesh = sq::res::mesh().get("Characters/Don")))
        model->mesh = sq::res::mesh().add("Characters/Don"),
        model->mesh->create("Characters/Don");
    if (!(model->skin = sq::res::skin().get("Characters/Don")))
        model->skin = sq::res::skin().add("Characters/Don"),
        model->skin->create("Characters/Don");

    sq::Animation* anim;
    if (!(anim = sq::res::anim().get("Characters/Don/Forwards")))
        anim = sq::res::anim().add("Characters/Don/Forwards"),
        anim->create("Characters/Don/Forwards");
    if (!(anim = sq::res::anim().get("Characters/Don/Backwards")))
        anim = sq::res::anim().add("Characters/Don/Backwards"),
        anim->create("Characters/Don/Backwards");
    if (!(anim = sq::res::anim().get("Characters/Don/Standing")))
        anim = sq::res::anim().add("Characters/Don/Standing"),
        anim->create("Characters/Don/Standing");
    model->skel.use_restPose(anim->poseVec[0]);
}

void Player::tick() {
    using KB = sf::Keyboard;

    posCrnt = posNext;

    /////
    if (KB::isKeyPressed(KB::PageUp)) posNext.z += 0.05f;
    if (KB::isKeyPressed(KB::PageDown)) posNext.z -= 0.05f;
    /////

    sq::Direction newMoveDir = sq::Direction::None;

    if (KB::isKeyPressed(KB::Right) && !KB::isKeyPressed(KB::Left)) {
        newMoveDir = sq::Direction::East;
        posNext += glm::rotateZ(vec3(0.08f, 0.f, 0.f), rotZ);
    } else if (KB::isKeyPressed(KB::Left) && !KB::isKeyPressed(KB::Right)) {
        newMoveDir = sq::Direction::West;
        posNext += glm::rotateZ(vec3(-0.08f, 0.f, 0.f), rotZ);
    }

    if (KB::isKeyPressed(KB::Up) && !KB::isKeyPressed(KB::Down)) {
        newMoveDir = sq::Direction::North;
        posNext += glm::rotateZ(vec3(0.f, 0.08f, 0.f), rotZ);
    } else if (KB::isKeyPressed(KB::Down) && !KB::isKeyPressed(KB::Up)) {
        newMoveDir = sq::Direction::South,
        posNext += glm::rotateZ(vec3(0.f, -0.08f, 0.f), rotZ);
    }

    if (moveDir != newMoveDir) {
        moveDir = newMoveDir;
        if (moveDir == sq::Direction::North) {
            model->skel.use_timeline(sq::res::anim().get("Characters/Don/Forwards")->timelineVec[0]);
            model->skel.play_anim(true, 2, 0);
        }
        if (moveDir == sq::Direction::South) {
            model->skel.use_timeline(sq::res::anim().get("Characters/Don/Backwards")->timelineVec[0]);
            model->skel.play_anim(true, 2, 0);
        }
        if (moveDir == sq::Direction::None) {
            model->skel.stop_anim(2);
        }
    }

    model->skel.tick();

}

void Player::calc(double _accum) {
    model->skel.calc(_accum);

    const double dt = 1.0 / 24.0;
    vec3 pos = glm::mix(posCrnt, posNext, _accum / dt);

    camera->pos = pos;
    camera->pos.z += zCam;

    pos.z = 0.f;

    if (app->settings.crnt<bool>("mouseFocus")) {
        vec2 mMove = app->mouse_relatify();
        rotZ = rotZ + mMove.x/600.f;
        rotX = glm::clamp(rotX + mMove.y/900.f, -1.1f, 1.1f);
        camera->dir = glm::rotateZ(glm::rotateX(vec3(0,1,0), rotX), rotZ);
    }

    camera->update();
    camera->recalc_frustums();

    model->matrix = glm::translate(mat4(), pos);
    model->matrix = glm::rotate(model->matrix, rotZ, {0,0,1});
    model->bbox = sq::bbox_by_model(model->mesh->bbox, model->matrix);
}
