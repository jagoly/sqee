#include <sqee/dop/Classes.hpp>

namespace dop = sq::dop;
namespace algo = sq::algo;

//============================================================================//

void dop::Group::reserve(size_t count)
{
    mIds.reserve(count);
}

void dop::Group::shrink_to_fit()
{
    mIds.shrink_to_fit();
}

void dop::Group::clear()
{
    mIsSorted = true;
    mIds.clear();
}

//============================================================================//

void dop::Group::insert(int32_t id)
{
    SQASSERT(algo::none_of(mIds, algo::pred_equal_to(id)), "id already used");

    if (mIsSorted == true && size() != 0u)
        mIsSorted = mIds.back() < id;

    mIds.push_back(id);
}

//============================================================================//

void dop::Group::erase(int32_t id)
{
    const auto iter = algo::find(mIds, id);
    SQASSERT(iter != mIds.end(), "id not found");

    mIds.erase(iter);
}
