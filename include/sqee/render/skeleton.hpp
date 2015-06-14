#pragma once
#include <sqee/forward.hpp>

#include <sqee/render/animation.hpp>

namespace sq {

struct Bone : NonCopyable {
    Bone(const Bone* _parent, fvec3 _head, fvec3 _tail, fquat _quat);
    const Bone* const parent; const fvec3 head, tail; const fquat quat;
};

struct BoneAnim : NonCopyable {
    BoneAnim(const BoneAnim* _parent, const Bone* _bone);
    const BoneAnim* const parent; const Bone* const bone;
    fquat rotation, cacheRotation;
    fvec3 offset, cacheOffset;
};

class Skeleton : NonCopyable {
public:
    Skeleton(uint _tickRate);
    const uint tickRate;

    vector<glm::mat3x4> boneMatVec;
    using Keyframe = Animation::Keyframe;
    using Timeline = Animation::Timeline;
    using Bone = Animation::Bone;
    using Pose = Animation::Pose;
    Pose calcPose, restPose;
    Timeline timeline;

    enum class State { Running, Ending, Paused, Done };
    State state = State::Done; bool looping = false;

    void revert_pose();
    void play_anim(uint _span, uint _spanEnd);
    void loop_anim(uint _span);
    void stop_anim(uint _span);
    void resume_anim();
    void pause_anim();

    void tick();
    void calc(double _accum);

private:
    uint span = 0u;
    uint index = 0u;
    uint ticks = 0u;
    uint spanEnd = 0u;
    Pose poseCrnt, poseNext;
};

}
