#pragma once
#include "forward.hpp"

#include <vector>

#include "misc/resholder.hpp"

namespace sq {

class Animation : NonCopyable {
public:
    void create(const string& _path);

    uint bCount = 0;
    uint pCount = 0;
    uint tCount = 0;

    using Pose = std::pair<std::vector<vec4>, std::vector<vec3>>;
    using Keyframe = std::pair<Pose&, uint>;
    using Timeline = std::vector<Keyframe>;
    std::vector<Pose> poseVec;
    std::vector<Timeline> timelineVec;
};

namespace res { ResHolder<Animation>& anim(); }

}
