#include <glm/gtc/quaternion.hpp>

#include "sqee/models/skeleton.hpp"

using namespace sq;

void Skeleton::use_restPose(Animation::Pose& _pose) {
    restPose = &_pose;
    quatVec = restPose->first;
    offsVec = restPose->second;
}

void Skeleton::use_timeline(Animation::Timeline& _timeline) {
    timeline = &_timeline;
}

void Skeleton::play_anim(bool _loop, uint _spanA, uint _spanB) {
    looping = _loop;
    span = _spanA;
    spanB = _spanB;
    running = true;
    index = -1;
    ticks = 0;

    transPose.first = quatVec;
    transPose.second = offsVec;
    poseCrnt = &transPose;
    poseNext = &(*timeline)[0].first;
}

void Skeleton::stop_anim(uint _span) {
    index = -2;
    ticks = 0;
    span = _span;

    transPose.first = quatVec;
    transPose.second = offsVec;
    poseCrnt = &transPose;
    poseNext = restPose;
}

void Skeleton::tick() {
    if (!running) return;
    ticks += 1;

    if (index == -1) { // starting
        if (ticks == span) {
            index = 1;
            poseCrnt = poseNext;
            poseNext = &(*timeline)[index].first;
            span = (*timeline)[index].second;
            ticks = 0;
        } return;
    }

    if (index == -2) { // stoping
        if (ticks == span) {
            quatVec = restPose->first;
            offsVec = restPose->second;
            running = false;
        } return;
    }

    if (ticks == span) {
        poseCrnt = poseNext;
        ticks = 0;
        index += 1;
        if (uint(index) == timeline->size()) {
            if (looping) index = 0;
            else { stop_anim(spanB); return; }
        }
    }

    if (running) {
        poseNext = &(*timeline)[index].first;
        span = (*timeline)[index].second;
    }
}

void Skeleton::calc(double _accum) {
    if (!running) return;

    const auto& q1 = poseCrnt->first;
    const auto& q2 = poseNext->first;
    const auto& o1 = poseCrnt->second;
    const auto& o2 = poseNext->second;
    quatVec.clear(); offsVec.clear();

    float percent = (ticks + _accum*double(tickRate)) / double(span);
    for (uint i = 0; i < restPose->first.size(); i++) {
        glm::quat a(q1[i].x, q1[i].y, q1[i].z, q1[i].w);
        glm::quat b(q2[i].x, q2[i].y, q2[i].z, q2[i].w);
        glm::quat ab(glm::slerp(a, b, percent));
        quatVec.emplace_back(ab.w, ab.x, ab.y, ab.z);
        offsVec.emplace_back(glm::mix(o1[i], o2[i], percent));
    }
}
