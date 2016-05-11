/*void ModelSkelly::refresh() {
    if (PROP_pose.empty() == false) {
        poseCalc = arma->poseMap.at(PROP_pose);
        auto uboData = sq::Armature::make_UboData(poseCalc);
        ubo.update("bones", uboData.data());
    }

    if (PROP_anim.empty() == false) {
        anim = arma->animMap.at(PROP_anim);
    }
}


void ModelSkelly::update() {
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
    Mat4F normMat(sq::make_normMat(world.camera->viewMat * matrix));
    ubo.update("normMat", &normMat);

    if (state == State::Done || state == State::Paused) return;
    poseCalc = sq::Armature::mix_Poses(poseCrnt, poseNext, float(_accum*24.f + ticks) / span);
    auto uboData = sq::Armature::make_UboData(poseCalc);
    ubo.update("bones", uboData.data());
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
*/
