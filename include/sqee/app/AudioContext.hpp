// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/Types.hpp>

namespace sq {

//====== Forward Declarations and Enums ======================================//

class Sound;

/// Groups of sounds that can be controlled together.
enum class SoundGroup : uint8_t
{
    Music = 1 << 0, ///< background music
    Menu  = 1 << 1, ///< menu effects
    Sfx   = 1 << 2, ///< game effects
    Other = 1 << 3, ///< unused extra group
};

/// Bitfield representing one or more sound groups.
struct SoundGroups
{
    constexpr SoundGroups() = default;
    constexpr SoundGroups(SoundGroup group) : value(uint8_t(group)) {}
    constexpr explicit SoundGroups(uint8_t value) : value(value) {}
    uint8_t value = 0u;
};

/// Bitwise OR operator for enum class type.
constexpr SoundGroups operator|(SoundGroup a, SoundGroup b) { return SoundGroups(uint8_t(a) | uint8_t(b)); }

//============================================================================//

/// Handles the initialisation and playback of audio.
class SQEE_API AudioContext final : private NonCopyable
{
public: //====================================================//

    AudioContext();

    ~AudioContext();

    //--------------------------------------------------------//

    /// Play a sound using the next available id.
    int64_t play_sound(const Sound& sound, SoundGroup group, float volume, bool loop);

    //--------------------------------------------------------//

    /// Pause or resume an individual sound.
    void set_sound_paused(int64_t id, bool pause);

    /// Remove an individual sound.
    void stop_sound(int64_t id);

    //--------------------------------------------------------//

    /// Do not start sounds from one or more groups.
    void set_groups_ignored(SoundGroups groups, bool ignore);

    /// Pause sounds from one or more sound groups.
    void set_groups_paused(SoundGroups groups, bool pause);

    /// Remove all sounds in the specified groups.
    void stop_groups(SoundGroups groups);

private: //===================================================//

    struct ActiveSound
    {
        int64_t id;
        const Sound* sound;
        SoundGroup group;
        float volume;
        bool loop;
        bool paused;
        size_t cursor;
    };

    std::vector<ActiveSound> mActiveSounds;

    int64_t mCurrentId = -1;

    SoundGroups mIgnoredGroups {};
    SoundGroups mPausedGroups {};

    void impl_reset_sound(const Sound* old, const Sound* fresh = nullptr);
    friend Sound;

    struct Implementation;
    std::unique_ptr<Implementation> impl;
    friend Implementation;
};

//============================================================================//

} // namespace sq
