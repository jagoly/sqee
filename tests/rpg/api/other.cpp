#include "other.hpp"

//============================================================================//

using namespace sqt::api;

//============================================================================//

void AnimationTimeline::set_times(const std::vector<uint32_t>& times)
{
    mTimes = times;
}

void AnimationTimeline::add_Transform(Vec3F position, QuatF rotation, float scale)
{
    mTransform.push_back({position, rotation, scale});
}

void AnimationTimeline::debug_assert_sanity()
{
    SQASSERT(mTimes.size() + 1u == mTransform.size(), "");
}
