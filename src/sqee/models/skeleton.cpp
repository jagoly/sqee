#include <cstring>

#include <glm/gtc/quaternion.hpp>

#include "app/logging.hpp"
#include "misc/files.hpp"
#include "models/skeleton.hpp"

using namespace sq;

void Skeleton::setup(Animation* _anim) {
    animCrnt = _anim;
    kfrCount = animCrnt->kfrVec.size();
    poseCrnt = &animCrnt->kfrVec.front().second;
    if (kfrCount > 1) {
        span = animCrnt->kfrVec.front().first;
        poseNext = &animCrnt->kfrVec[1].second;
    }
}

void Skeleton::tick() {
    if (animNext != nullptr) {
        if (++progress == span) {
            animCrnt = animNext;
            animNext = nullptr;
            kfrCount = animCrnt->kfrVec.size();
            poseCrnt = poseNext;
            if (kfrCount > 1) {
                span = animCrnt->kfrVec[kfrInd].first;
                poseNext = &animCrnt->kfrVec[++kfrInd].second;
                progress = 0;
            }
        }
    } else {
        if (kfrCount > 1) {
            if (++progress == span) {
                if (kfrInd == kfrCount) kfrInd = 0;
                uint kfrNext = kfrInd+1;
                if (kfrNext == kfrCount) kfrNext = 0;

                span = animCrnt->kfrVec[kfrInd].first;
                poseCrnt = &animCrnt->kfrVec[kfrInd].second;
                poseNext = &animCrnt->kfrVec[kfrNext].second;

                progress = 0;
                kfrInd++;
            }
        } else {
            std::memcpy(quatData, poseCrnt->quatData, animCrnt->bCount * 4 * sizeof(float));
            std::memcpy(offsData, poseCrnt->offsData, animCrnt->bCount * 3 * sizeof(float));
        }
    }
}


void Skeleton::calc(double _accum) {
    if (kfrCount > 1) {
        const float* q1 = poseCrnt->quatData;
        const float* q2 = poseNext->quatData;
        const float* o1 = poseCrnt->offsData;
        const float* o2 = poseNext->offsData;

        float percent = (progress + _accum * 24.0) / double(span);

        for (uint i = 0; i < animCrnt->bCount; i++) {
            glm::quat quatA(q1[i*4+0], q1[i*4+1], q1[i*4+2], q1[i*4+3]);
            glm::quat quatB(q2[i*4+0], q2[i*4+1], q2[i*4+2], q2[i*4+3]);
            glm::quat quatC(glm::slerp(quatA, quatB, percent));
            quatData[i*4+0] = quatC.w;
            quatData[i*4+1] = quatC.x;
            quatData[i*4+2] = quatC.y;
            quatData[i*4+3] = quatC.z;

            vec3 offsA(o1[i*3+0], o1[i*3+1], o1[i*3+2]);
            vec3 offsB(o2[i*3+0], o2[i*3+1], o2[i*3+2]);
            vec3 offsC(glm::mix(offsA, offsB, percent));
            offsData[i*3+0] = offsC.x;
            offsData[i*3+1] = offsC.y;
            offsData[i*3+2] = offsC.z;
        }
    }
}

void Skeleton::transition(Animation* _anim, uint _kfrInd, uint _span) {
    animNext = _anim;
    kfrInd = _kfrInd;
    poseNext = &animNext->kfrVec[kfrInd].second;
    span = _span;
    progress = 0;
    kfrCount = 2;
}
