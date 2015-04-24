#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Animation : NonCopyable {
public:
    void create(const string& _path);

    uint bCount = 0;
    uint pCount = 0;
    uint tCount = 0;

    using Pose = pair<vector<vec4>, vector<vec3>>;
    using Keyframe = pair<Pose&, uint>;
    using Timeline = vector<Keyframe>;
    vector<Pose> poseVec;
    vector<Timeline> timelineVec;
};

namespace res {
ResHolder<Animation>& anim();
string& anim_path();
}

}
