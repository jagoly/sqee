#pragma once

#include "../systems/WorldStuff.hpp"

//============================================================================//

namespace sqt { namespace api {

//============================================================================//

struct AnimationTimeline
{
    std::vector<uint32_t> mTimes;
    std::vector<sys::AnimTransformData> mTransform;

    void set_times(const std::vector<uint32_t>& times);

    void add_Transform(Vec3F position, QuatF rotation, float scale);

    void debug_assert_sanity();
};

//============================================================================//

}} // namespace sqt::api
