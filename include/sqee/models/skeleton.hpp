#pragma once
#include "forward.hpp"

#include <vector>

#include "models/animation.hpp"

namespace sq {

class Skeleton : NonCopyable {
public:
    void setup(Animation* _anim);

    float quatData[40*4];
    float offsData[40*3];

    void tick();
    void jump(int _frame);
    void pause();
    void calc(double _accum);

    void transition(Animation* _anim, uint _kfrInd, uint _span);

protected:
    Animation* animCrnt;
    Animation* animNext = nullptr;

    uint kfrCount;

    Animation::Pose* poseCrnt;
    Animation::Pose* poseNext = nullptr;

    uint span;

    uint progress = 0;
    uint kfrInd = 0;
};

}
