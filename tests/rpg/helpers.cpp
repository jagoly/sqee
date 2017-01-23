/*#include "helpers.hpp"

namespace sqt {
namespace dop {

void sort(const IdSet& constIds)
{
    // check if set is already sorted
    if (constIds.mIsSorted == true) return;

    // remove const and do the sort
    auto& ids = const_cast<IdSet&>(constIds);
    detail::insertion_sort(ids.mVec);

    ids.mIsSorted = true;
}

template <class... Sets>
IdSet reduce(const Sets&... sets)
{
    // check if any of the sets are empty
    if (variadic_or(sets.mVec.empty()...))
        return IdSet();

    sq::expand {(dop::sort(sets), 0)...};
    constexpr uint setCount = sizeof...(Sets);
    IdSet result(maths::min(sets.size()...));

    // value-initialize to zero
    std::array<uint, setCount> indices {};

    // for easy iteration through id vectors
    auto ptrs = detail::make_pointer_array(sets...);

    // id of the entry attempting to be added
    int32_t currentID = maths::max(sets.mVec.front()...);

    while (true)
    {
        label_loop_continue:

        // find an id shared by all tables
        for (uint num = 0u; num < setCount; ++num)
        {
            const auto& vec = ptrs[num]->mVec;

            if (vec[indices[num]] > currentID)
            {
                currentID = vec[indices[num]];
                goto label_loop_continue;
            }

            while (vec[indices[num]] < currentID)
                if (++indices[num] == vec.size())
                    goto label_loop_break;
        }

        // add shared id to the result vector
        result.mVec.push_back(currentID);

        // try to increment each of the indices
        for (uint num = 0u; num < setCount; ++num)
            if (++indices[num] == ptrs[num]->size())
                goto label_loop_break;
    }

    label_loop_break:

    result.shrink_to_fit();
    return result;
}

template IdSet reduce(const IdSet&, const IdSet&);
template IdSet reduce(const IdSet&, const IdSet&, const IdSet&);
template IdSet reduce(const IdSet&, const IdSet&, const IdSet&, const IdSet&);

}} // namespace sqt::dop
*/
