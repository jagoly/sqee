#pragma once
#include "forward.hpp"

#include <utility>
#include <vector>

#include "misc/resholder.hpp"

namespace sq {

class Animation : NonCopyable {
public:
    struct Pose {
        Pose(uint _bCount, const float* _quatData, const float* _offsData);
        float quatData[40*4];
        float offsData[40*3];
    };

    void create(const string& _filePath);

    uint bCount;
    uint pCount = 0;
    uint kCount = 0;
    std::vector<std::pair<uint, Pose&>> kfrVec;

private:
    std::vector<Pose> poseVec;
};

namespace res { ResHolder<Animation>& animation(); }

}
