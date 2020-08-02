#include "Animation.hpp"

#include <sqee/misc/DopFunctions.hpp>

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

//============================================================================//

void sqt::sys::system_tick_animations(WorldStuff& stuff)
{
    for (auto& [id, meta] : dop::joined(stuff.tables.animation))
    {
        if (++meta.progress == meta.times[meta.index])
        {
            meta.progress = 0u;

            if (++meta.index == meta.times.size())
            {
                if (meta.callback_on_end)
                    meta.callback_on_end(id);

                meta.index = 0u;
            }
        }
    }
}

//============================================================================//

void sqt::sys::system_blend_animations(WorldStuff& stuff, float tickBlend)
{
    const auto& metaTable = stuff.tables.animation;
    const auto& timelines = stuff.animation;

    auto& tables = stuff.tables;

    //--------------------------------------------------------//

    for ( auto& [id, meta, timeline, transform] : dop::joined(metaTable, timelines.transform, tables.transform) )
    {
        const float factor = (tickBlend + float(meta.progress)) / float(meta.times[meta.index]);

        const auto& current = timeline[meta.index];
        const auto& next = timeline[meta.index + 1u];

        transform.localPosition = maths::mix(current.position, next.position, factor);
        transform.localRotation = maths::slerp(current.rotation, next.rotation, factor);
        transform.localScale = maths::mix(current.scale, next.scale, factor);
    }
}
