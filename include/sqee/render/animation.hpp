#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Animation : NonCopyable {
public:
    void create(const string& _path);

    uint bCount = 0u;
    uint pCount = 0u;
    uint tCount = 0u;

    struct Bone { fquat quat; fvec3 offs; };
    using Pose = vector<Bone>;
    vector<Pose> poseVec;

    using Keyframe = pair<Pose, uint>;
    using Timeline = vector<Keyframe>;
    vector<Timeline> timelineVec;

    static glm::mat3x4 to_matrix(Bone _bone);
};

namespace res {
ResHolder<Animation>& anim();
string& anim_path();
}

}
