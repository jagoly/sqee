#include <sqee/app/Resources.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelSkelly.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

ModelSkelly::ModelSkelly(const string& _name, Cell* _cell)
    : Object(typeid(ModelSkelly), _name, _cell) {

    ubo.reserve("matrix", 16u);
    ubo.reserve("normMat", 16u);
    ubo.reserve("bones", 960u);
    ubo.allocate_storage();
}


void ModelSkelly::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("shadow", PROP_shadow);
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
    if (invalid == false) return;

    if ((arma = sq::static_Armature().get(PROP_arma)) == nullptr)
        arma = sq::static_Armature().add(PROP_arma, PROP_arma);

    if ((mesh = sq::static_Mesh().get(PROP_mesh)) == nullptr)
        mesh = sq::static_Mesh().add(PROP_mesh, PROP_mesh);

    if ((skin = sq::static_Skin().get(PROP_skin)) == nullptr)
        skin = sq::static_Skin().add(PROP_skin, PROP_skin);

    if (PROP_pose.empty() == false) {
        poseCalc = arma->poseMap.at(PROP_pose);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo.update("bones", uboData.data());
    }

    if (PROP_anim.empty() == false) {
        anim = arma->animMap.at(PROP_anim);
    }

    animate(); invalid = false;
}


void ModelSkelly::update() {
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
            ubo.update("bones", uboData.data());
        } return;
    }
}


void ModelSkelly::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate();

    Mat4F normMat(sq::make_normMat(world.camera->viewMat * matrix));
    ubo.update("normMat", &normMat);

    if (state == State::Done || state == State::Paused) return;
    poseCalc = sq::Armature::mix_Poses(poseCrnt, poseNext, float(_accum*24.f + ticks) / span);
    auto uboData = sq::Armature::make_UboData(poseCalc);
    ubo.update("bones", uboData.data());
}


void ModelSkelly::animate() {
    matrix = maths::translate(Mat4F(), PROP_position + cell->PROP_position);
    matrix *= Mat4F(PROP_rotation); matrix = maths::scale(matrix, PROP_scale);
    sphere = sq::make_Sphere(matrix, mesh->origin, mesh->radius);
    negScale = maths::determinant(matrix) < 0.f;
    ubo.update("matrix", &matrix);
}


void ModelSkelly::FUNC_stop(uint _time) {
    if (_time == 0u) {
        state = State::Done;
        poseCalc = arma->poseMap.at(PROP_pose);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo.update("bones", uboData.data());
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
