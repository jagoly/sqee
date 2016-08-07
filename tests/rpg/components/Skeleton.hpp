#pragma once

#include <sqee/ecs/Component.hpp>

namespace sqt {

class SkeletonComponent : public sq::Component {
public:
    SkeletonComponent(sq::Entity&) {}
    static string type() { return "Skeleton"; }

    /// name of the armature used
    string PROP_armature = "";

    /// name of the pose used
    string PROP_pose = "";
};

}
