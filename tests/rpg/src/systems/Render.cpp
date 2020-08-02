#include "Render.hpp"

#include <sqee/misc/DopFunctions.hpp>

namespace algo = sq::algo;

//============================================================================//

void sqt::sys::system_refresh_combo_sets(WorldStuff& stuff)
{
    stuff.groups.modelSimple.clear();
    stuff.groups.modelSkelly.clear();
    stuff.groups.lightOrtho.clear();
    stuff.groups.lightPoint.clear();
    stuff.groups.lightSpot.clear();

    // sort input data for faster insertion
    sq::dop::sort(stuff.tables.entity);

    for (int32_t id : stuff.tables.entity.mIds)
    {
        bool hasTransform  = stuff.tables.transform.exists(id);
        bool hasModel      = stuff.tables.model.exists(id);
        bool hasSkeleton   = stuff.tables.skeleton.exists(id);
        bool hasOrthoLight = stuff.tables.ortholight.exists(id);
        bool hasPointLight = stuff.tables.pointlight.exists(id);
        bool hasSpotLight  = stuff.tables.spotlight.exists(id);

        if (hasTransform && hasModel && !hasSkeleton)
            stuff.groups.modelSimple.mIds.push_back(id);

        if (hasTransform && hasModel && hasSkeleton)
            stuff.groups.modelSkelly.mIds.push_back(id);

        if (hasTransform && hasOrthoLight)
            stuff.groups.lightOrtho.mIds.push_back(id);

        if (hasTransform && hasPointLight)
            stuff.groups.lightPoint.mIds.push_back(id);

        if (hasTransform && hasSpotLight)
            stuff.groups.lightSpot.mIds.push_back(id);
    }
}
