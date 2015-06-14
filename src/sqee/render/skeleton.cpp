#include <glm/mat3x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <sqee/render/skeleton.hpp>

#include <iostream>

using namespace sq;


Bone::Bone(const Bone* _parent, fvec3 _head, fvec3 _tail, fquat _quat)
    : parent(_parent), head(_head), tail(_tail), quat(_quat) {}

BoneAnim::BoneAnim(const BoneAnim* _parent, const Bone* _bone)
    : parent(_parent), bone(_bone) {}


Skeleton::Skeleton(uint _tickRate) : tickRate(_tickRate) {}

void Skeleton::revert_pose() {
    state = State::Done;
    calcPose = restPose;
    boneMatVec.clear();
    for (const auto& bone : calcPose)
        boneMatVec.emplace_back(Animation::to_matrix(bone));
}

void Skeleton::play_anim(uint _span, uint _spanEnd) {
    state = State::Running;
    spanEnd = _spanEnd;
    span = _span;
    index = 0u;
    ticks = 0u;
    looping = false;
    poseCrnt = calcPose;
    poseNext = timeline.front().first;
}

void Skeleton::loop_anim(uint _span) {
    state = State::Running;
    span = _span;
    index = 0u;
    ticks = 0u;
    looping = true;
    poseCrnt = calcPose;
    poseNext = timeline.front().first;
}

void Skeleton::stop_anim(uint _span) {
    state = State::Ending;
    span = _span;
    index = 0u;
    ticks = 0u;
    looping = false;
    poseCrnt = calcPose;
    poseNext = restPose;
}

void Skeleton::tick() {
    if (state == State::Done || state == State::Paused) return;
    ticks += 1u;

    if (state == State::Running) {
        if (ticks == span) {
            poseCrnt = poseNext;
            ticks = 0u; index += 1;
            if (index == timeline.size()) {
                if (looping) index = 0u;
                else stop_anim(spanEnd);
            } else {
                poseNext = timeline[index].first;
                span = timeline[index].second;
            }
        }
    }

    if (state == State::Ending) {
        if (ticks == span) {
            state = State::Done;
        } return;
    }
}

void Skeleton::calc(double _accum) {
    if (state == State::Done || state == State::Paused) return;
    calcPose.clear(); boneMatVec.clear();

    float percent = (ticks + _accum*tickRate) / span;
    for (uint i = 0; i < restPose.size(); i++) {
        const Bone& boneA = poseCrnt[i];
        const Bone& boneB = poseNext[i];
        fquat quat = glm::slerp(boneA.quat, boneB.quat, percent);
        fvec3 offs = glm::mix(boneA.offs, boneB.offs, percent);
        Bone bone {quat, offs}; calcPose.emplace_back(bone);
        glm::mat3x4 boneMat = Animation::to_matrix(bone);
        boneMatVec.emplace_back(boneMat);
    }
}
