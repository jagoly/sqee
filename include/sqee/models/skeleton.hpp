#pragma once

#include <maths/glm.hpp>
#include <misc/containers.hpp>

namespace sq {

class Animation : NonCopyable {
public:
    struct Pose {
        Pose(uint _bCount, const float* _quatData, const float* _offsData) {
            std::memcpy(quatData, _quatData, _bCount*4*sizeof(float));
            std::memcpy(offsData, _offsData, _bCount*3*sizeof(float));
        }
        float quatData[40*4];
        float offsData[40*3];
    };

    void load(const string& _filePath);

    uint bCount;
    uint pCount = 0;
    uint kCount = 0;
    vector<pair<uint, Pose&>> kfrVec;

private:
    vector<Pose> poseVec;
};
typedef ResHolder<string, Animation> AnimHolder;


class Skeleton {
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
