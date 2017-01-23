#include <sqee/sound/Sound.hpp>

#include "Sound.hpp"

using namespace sqt::sys;

//============================================================================//


// TODO: get rid of this crap

template<class VectorA, class VectorB, class Predicate> inline
void multi_erase_if(VectorA& vecA, VectorB& vecB, const Predicate& pred)
{
    SQASSERT(vecA.size() == vecB.size(), "sizes do not match");

    const auto iter = sq::algo::find_if(vecA, pred);
    auto first = size_t(iter - vecA.begin());

    const size_t vectorSize = vecA.size();
    if (first == vectorSize) return;

    for (size_t i = first; ++i != vectorSize;)
    {
        if (pred(vecA[i])) continue;

        vecA[first] = std::move(vecA[i]);
        vecB[first] = std::move(vecB[i]);

        first += 1u;
    }

    vecA.erase(vecA.begin() + first, vecA.end());
    vecB.erase(vecB.begin() + first, vecB.end());
}


void sqt::sys::system_refresh_sounds(WorldStuff& stuff)
{
    auto& table = stuff.sound.table;

    auto predicate = [](const auto& s) { return s.sound.check_stopped(); };
    multi_erase_if(table.mData, table.mIds, predicate);

    for (auto& item : table.mData)
    {
        if (item.entity != -1)
        {
            auto& transform = stuff.tables.transform.get(item.entity);
            item.sound.set_position(transform.worldPosition);
        }
    }
}
