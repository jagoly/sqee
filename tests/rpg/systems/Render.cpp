#include <sqee/dop/Functions.hpp>

#include "Render.hpp"

namespace algo = sq::algo;

//============================================================================//

void sqt::sys::system_refresh_combo_sets(WorldStuff& stuff)
{
    stuff.groups.modelSimple.clear();
    stuff.groups.modelSkelly.clear();
    stuff.groups.lightSpot.clear();

    // sort input data for faster insertion
    sq::dop::sort(stuff.tables.entity);

    for (int32_t id : stuff.tables.entity.mIds)
    {
        bool hasTransform = algo::exists(stuff.tables.transform.mIds, id);
        bool hasModel     = algo::exists(stuff.tables.model.mIds, id);
        bool hasSkeleton  = algo::exists(stuff.tables.skeleton.mIds, id);
        bool hasLightSpot = algo::exists(stuff.tables.spotLight.mIds, id);

        if (hasTransform && hasModel && !hasSkeleton)
            stuff.groups.modelSimple.mIds.push_back(id);

        if (hasTransform && hasModel && hasSkeleton)
            stuff.groups.modelSkelly.mIds.push_back(id);

        if (hasTransform && hasLightSpot)
            stuff.groups.lightSpot.mIds.push_back(id);
    }
}
