#pragma once
#include <sqee/forward.hpp>

#include <sqee/render/animation.hpp>

namespace sq {

class Skeleton : NonCopyable {
public:
    uint tickRate = 60;

    void use_restPose(Animation::Pose& _pose);
    void use_timeline(Animation::Timeline& _timeline);
    void play_anim(bool _looped, uint _spanA, uint _spanB);
    void stop_anim(uint _span);
    void pause_anim();

    vector<fvec4> quatVec;
    vector<fvec3> offsVec;

    void tick();
    void calc(double _accum);

private:
    Animation::Pose* restPose;
    Animation::Timeline* timeline;
    int index = 0;
    uint ticks = 0;
    bool looping = false;
    bool running = false;

    uint span = 0;
    uint spanB = 0;

    Animation::Pose* poseCrnt = nullptr;
    Animation::Pose* poseNext = nullptr;
    Animation::Pose transPose;
};

}
