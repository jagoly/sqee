#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelSkelly.hpp"

using namespace sqt::wcoe;

ModelSkelly::ModelSkelly(const string& _name, Cell* _cell)
    : Object(ObjType::ModelSkelly, _name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normMat", 16);
    ubo->reserve("bones", 12*80);
    ubo->create();
}


void ModelSkelly::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("rotation", 4);
    SPEC_ASSERT_FLOAT("scale", 3);
    SPEC_ASSERT_STRING("arma", 1);
    SPEC_ASSERT_STRING("mesh", 1);
    SPEC_ASSERT_STRING("skin", 1);

    PROP_shadow   = SPEC_HAS_FLAG("shadow");
    PROP_render   = SPEC_HAS_FLAG("render");
    PROP_position = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_rotation = glm::make_quat(_spec.fMap.at("rotation").data());
    PROP_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    PROP_armaPath = _spec.sMap.at("arma")[0];
    PROP_meshPath = _spec.sMap.at("mesh")[0];
    PROP_skinPath = _spec.sMap.at("skin")[0];
    SPEC_IF_STRING("pose") PROP_poseName = _spec.sMap.at("pose")[0];
    SPEC_IF_STRING("anim") PROP_animName = _spec.sMap.at("pose")[0];
}


void ModelSkelly::refresh() {
    if ((arma = sq::res::arma().get(PROP_armaPath)) == nullptr)
        arma = sq::res::arma().add(PROP_armaPath),
        arma->create(PROP_armaPath);

    if ((mesh = sq::res::mesh().get(PROP_meshPath)) == nullptr)
        mesh = sq::res::mesh().add(PROP_meshPath),
        mesh->create(PROP_meshPath);

    if ((skin = sq::res::skin().get(PROP_skinPath)) == nullptr)
        skin = sq::res::skin().add(PROP_skinPath),
        skin->create(PROP_skinPath);

    if (PROP_poseName.empty() == false) {
        poseCalc = arma->poseMap.at(PROP_poseName);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo->bind(1); ubo->update("bones", uboData.data());
    }

    animate();
}


void ModelSkelly::tick() {
    bool doFinish = false;
    if (ANIM_position.active()) if (ANIM_position.tick()) doFinish = true;
    if (ANIM_rotation.active()) if (ANIM_rotation.tick()) doFinish = true;
    if (ANIM_scale.active())    if (ANIM_scale.tick())    doFinish = true;
    if (doFinish == true) animate();

    if (state == State::Done || state == State::Paused) return;
    ticks += 1u;

    if (state == State::Running) {
        if (ticks == span) {
            poseCrnt = poseNext;
            ticks = 0u; index += 1u;
            if (index == anim.size()) {
                if (looping == true) {
                    index = 0u; span = anim[index].second;
                    poseNext = anim[index].first;
                } else FUNC_stop(spanEnd);
            } else { span = anim[index].second;
                poseNext = anim[index].first;
            }
        }
    }

    if (state == State::Ending) {
        if (ticks == span) {
            state = State::Done;
            poseCalc = arma->poseMap.at(PROP_poseName);
            auto uboData = sq::Armature::make_UboData(poseCalc);
            ubo->bind(1); ubo->update("bones", uboData.data());
        } return;
    }
}


void ModelSkelly::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate(); else ubo->bind(1);

    fmat4 normMat(sq::make_normMat(cell->world->camera->viewMat * matrix));
    ubo->update("normMat", &normMat);

    if (state == State::Done || state == State::Paused) return;
    poseCalc = sq::Armature::mix_Poses(poseCrnt, poseNext, (ticks + _accum*24.f) / span);
    auto uboData = sq::Armature::make_UboData(poseCalc); ubo->update("bones", uboData.data());
}


void ModelSkelly::animate() {
    matrix = glm::translate(fmat4(), PROP_position + cell->DAT_position);
    matrix *= glm::mat4_cast(PROP_rotation); matrix = glm::scale(matrix, PROP_scale);
    sphere = sq::make_Sphere(matrix, mesh->origin, mesh->radius);
    ubo->bind(1); ubo->update("matrix", &matrix);
    negScale = glm::determinant(matrix) < 0.f;
}


void ModelSkelly::FUNC_stop(uint _time) {
    if (_time == 0u) {
        state = State::Done;
        poseCalc = arma->poseMap.at(PROP_poseName);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo->bind(1); ubo->update("bones", uboData.data());
    } else {
        state = State::Ending; span = _time;
        looping = false; index = 0u; ticks = 0u;
        poseNext = arma->poseMap.at(PROP_poseName);
        poseCrnt = poseCalc;
    }
}


void ModelSkelly::FUNC_loop(uint _time) {
    anim = arma->animMap.at(PROP_animName);
    looping = true; ticks = 0u;
    state = State::Running;

    if (_time == 0u) {
        poseCrnt = anim.front().first;
        poseNext = anim[1].first;
        span = anim[0].second;
        index = 1u;
    } else {
        poseCrnt = poseCalc;
        poseNext = anim.front().first;
        span = _time; index = 0u;
    }
}


void ModelSkelly::FUNC_play(uint _timeA, uint _timeB) {
    anim = arma->animMap.at(PROP_animName);
    looping = false; ticks = 0u;
    state = State::Running;
    spanEnd = _timeB;

    if (_timeA == 0u) {
        poseCrnt = anim.front().first;
        poseNext = anim[1].first;
        span = anim[0].second;
        index = 1u;
    } else {
        poseCrnt = poseCalc;
        poseNext = anim.front().first;
        span = _timeA; index = 0u;
    }
}
