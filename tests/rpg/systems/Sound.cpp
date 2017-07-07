#include <sqee/sound/Sound.hpp>

#include "Sound.hpp"

using namespace sqt::sys;

//============================================================================//

void sqt::sys::system_refresh_sounds(WorldStuff& stuff)
{
    stuff.soundTable.erase_if([](int32_t, auto& data) { return data.sound.check_stopped(); });

    for (auto& data : stuff.soundTable.mData)
    {
        if (data.entity != -1)
        {
            auto& transform = stuff.tables.transform.get(data.entity);
            data.sound.set_position(transform.worldPosition);
        }
    }
}
