#include <glm/gtc/matrix_transform.hpp>

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
    _spec.set_if("shadow", PROP_shadow);
    _spec.set_if("render", PROP_render);
    _spec.set_if("probes", PROP_probes);
    _spec.set_if("position", PROP_position);
    _spec.set_if("rotation", PROP_rotation);
    _spec.set_if("scale", PROP_scale);
    _spec.set_if("mesh", PROP_mesh);
    _spec.set_if("skin", PROP_skin);
    _spec.set_if("arma", PROP_arma);
    _spec.set_if("pose", PROP_pose);
    _spec.set_if("anim", PROP_anim);
}


void ModelSkelly::refresh() {
    if (invalid == true) {
        if ((arma = sq::res::arma().get(PROP_arma)) == nullptr)
            arma = sq::res::arma().add(PROP_arma),
            arma->create(PROP_arma);

        if ((mesh = sq::res::mesh().get(PROP_mesh)) == nullptr)
            mesh = sq::res::mesh().add(PROP_mesh),
            mesh->create(PROP_mesh);

        if ((skin = sq::res::skin().get(PROP_skin)) == nullptr)
            skin = sq::res::skin().add(PROP_skin),
            skin->create(PROP_skin);

        if (PROP_pose.empty() == false) {
            poseCalc = arma->poseMap.at(PROP_pose);
            auto uboData = sq::Armature::make_UboData(poseCalc);
            ubo->bind(1); ubo->update("bones", uboData.data());
        }

        if (PROP_anim.empty() == false) {
            anim = arma->animMap.at(PROP_anim);
        }

        invalid = false;
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
            poseCalc = arma->poseMap.at(PROP_pose);
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
        poseCalc = arma->poseMap.at(PROP_pose);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo->bind(1); ubo->update("bones", uboData.data());
    } else {
        state = State::Ending; span = _time;
        looping = false; index = 0u; ticks = 0u;
        poseNext = arma->poseMap.at(PROP_pose);
        poseCrnt = poseCalc;
    }
}


void ModelSkelly::FUNC_loop(uint _time) {
    anim = arma->animMap.at(PROP_anim);
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
    anim = arma->animMap.at(PROP_anim);
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
