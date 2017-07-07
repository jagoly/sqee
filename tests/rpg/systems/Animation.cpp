#include <sqee/dop/Functions.hpp>

#include "Animation.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

//============================================================================//

void sqt::sys::system_tick_animations(WorldStuff& stuff)
{
    for (auto& [id, data] : dop::joined(stuff.tables.animation))
    {
        if (++data.progress == data.times[data.index])
        {
            data.progress = 0u;

            if (++data.index == data.times.size())
            {
                if (data.callback_on_end)
                    data.callback_on_end(id);
                data.index = 0u;
            }
        }
    }

//    for (auto& entry : dop::joined(metaTable, timelines.transform))
//    {
//        //auto& [id, meta, timeline] = joinEntry;

//        const auto& id = std::get<0>(entry);
//        const auto& meta = std::get<1>(entry);
//        const auto& timeline = std::get<2>(entry);

//        const auto& state = timeline[meta.index];
//        auto& transform = stuff.tables.transform.get(id);

//        transform.localPosition = state.position;
//        transform.localRotation = state.rotation;
//        transform.localScale = state.scale;
//    }
}

//============================================================================//

void sqt::sys::system_blend_animations(WorldStuff& stuff, float factor)
{
    const auto& table = stuff.tables.animation;
    const auto& timelines = stuff.animation;
    auto& tables = stuff.tables;

    for (auto& [id, meta, timeline, transform] : dop::joined(table, timelines.transform, tables.transform))
    {
        factor += float(meta.progress);
        factor /= float(meta.times[meta.index]);

        const auto& crnt = timeline[meta.index];
        const auto& next = timeline[meta.index + 1u];

        transform.localPosition = maths::mix(crnt.position, next.position, factor);
        transform.localRotation = maths::slerp(crnt.rotation, next.rotation, factor);
        transform.localScale = maths::mix(crnt.scale, next.scale, factor);
    }
}
