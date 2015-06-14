#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/animation.hpp>
#include <sqee/render/skeleton.hpp>
#include <sqee/render/camera.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include <iostream>

#include "../cell.hpp"
#include "../world.hpp"
#include "modelskelly.hpp"

using namespace sqt::wcoe;

ModelSkelly::ModelSkelly(const string& _name, Cell* _cell)
    : Object(ObjType::ModelSkelly, _name, _cell) {
    skel.reset(new sq::Skeleton(24u));
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normMat", 16);
    ubo->reserve("bones", 12*80);
    ubo->create();

    sq::Animation* anim = nullptr;
    if (!(anim = sq::res::anim().get("Characters/Sqeen/Resting")))
        anim = sq::res::anim().add("Characters/Sqeen/Resting"),
        anim->create("Characters/Sqeen/Resting");

    skel->restPose = anim->poseVec[0];
    skel->revert_pose();
}

void ModelSkelly::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("rotation", 4);
    SPEC_ASSERT_FLOAT("scale", 3);
    SPEC_ASSERT_STRING("mesh", 1);
    SPEC_ASSERT_STRING("skin", 1);

    PROP_shadow   = SPEC_HAS_FLAG("shadow");
    PROP_render   = SPEC_HAS_FLAG("render");
    PROP_position = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_rotation = glm::make_quat(_spec.fMap.at("rotation").data());
    PROP_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    PROP_mPath    = _spec.sMap.at("mesh")[0];
    PROP_sPath    = _spec.sMap.at("skin")[0];
}

void ModelSkelly::refresh() {
    if (!(mesh = sq::res::mesh().get(PROP_mPath)))
        mesh = sq::res::mesh().add(PROP_mPath),
        mesh->create(PROP_mPath);

    if (!(skin = sq::res::skin().get(PROP_sPath)))
        skin = sq::res::skin().add(PROP_sPath),
        skin->create(PROP_sPath);
}

void ModelSkelly::tick() {
    skel->tick();

    if (true) animate();
}

void ModelSkelly::calc(double _accum) {
    skel->calc(_accum);

    if (true) animate(); else ubo->bind(1);

    ubo->update("bones", skel->boneMatVec.data());
    fmat4 normMat(sq::make_normMat(cell->world->camera->viewMat * matrix));
    ubo->update("normMat", &normMat);
}

void ModelSkelly::animate() {
    matrix = glm::translate(fmat4(), PROP_position + cell->DAT_position);
    matrix *= glm::mat4_cast(PROP_rotation); matrix = glm::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->size, mesh->radius);
    ubo->bind(1); ubo->update("matrix", &matrix);
    negScale = glm::determinant(matrix) < 0.f;
}

void ModelSkelly::debug_A() {
    auto anim = sq::res::anim().get("Characters/Sqeen/Resting");
    skel->restPose = anim->poseVec[0];
    skel->stop_anim(48u);
}

void ModelSkelly::debug_B() {
    auto anim = sq::res::anim().get("Characters/Sqeen/Resting");
    skel->restPose = anim->poseVec[1];
    skel->stop_anim(48u);
}

void ModelSkelly::debug_C() {
    auto anim = sq::res::anim().get("Characters/Sqeen/Resting");
    skel->timeline = anim->timelineVec[0];
    skel->play_anim(6u, 12u);
}

void ModelSkelly::debug_D() {
    auto anim = sq::res::anim().get("Characters/Sqeen/Resting");
    skel->timeline = anim->timelineVec[0];
    skel->loop_anim(6u);
}
