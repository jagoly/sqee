#pragma once

#include <sqee/builtins.hpp>
#include <sqee/sound/Sound.hpp>
#include <sqee/ecs/Entity.hpp>

#include "Common.hpp"

namespace sqt {

class SoundSystem
{
public:

    using SoundID = int32_t;
    //using StreamID = int32_t;
    using EntityID = const sq::Entity*;

    SoundSystem();

    // play and stream methods /////

    /// play a non-positional sound
    SoundID play(const string& path, uint8_t group);

    /// play a sound at a fixed position
    SoundID play(const string& path, uint8_t group, Vec3F position);

    /// play a sound that follows an entity
    SoundID play(const string& path, uint8_t group, EntityID entity);

    /// stream a non-positional sound
    //StreamID stream(const string& path, uchar group);

    // modify paramater methods /////

    /// set the volume of a sound
    void set_sound_volume(SoundID id, float volume);

    /// set the volume of a sound
    void set_sound_loop(SoundID id, bool loop);

    /// set the volume multiplier of a group
    void set_group_volume(uint8_t group, float volume);

    // system methods /////

    void system_refresh();

private:

    struct SoundData
    {
        sq::Sound sound;
        uint8_t group;
        EntityID entity;
        float volume;
    };

    DataTable<SoundData> mSoundTable;

    // 0: EFFECTS | 1: SPEECH | 2: MUSIC /////
    array<float, 3> mGroupVolumes {{1.f, 1.f, 1.f}};

    void impl_play(const string& path, uint8_t group);
};

}
