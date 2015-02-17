#pragma once
#include "forward.hpp"

#include <utility>
#include <vector>

namespace sq {

class Animation : NonCopyable {
public:
    struct Pose {
        Pose(uint _bCount, const float* _quatData, const float* _offsData);
        float quatData[40*4];
        float offsData[40*3];
    };

    void load(const string& _filePath);

    uint bCount;
    uint pCount = 0;
    uint kCount = 0;
    std::vector<std::pair<uint, Pose&>> kfrVec;

private:
    std::vector<Pose> poseVec;
};


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
