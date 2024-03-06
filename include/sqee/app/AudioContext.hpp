#pragma once

#include <sqee/setup.hpp>

namespace sq { //###############################################################

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

//==============================================================================

/// Handles the initialisation and playback of audio.
class SQEE_API AudioContext
{
public: //======================================================

    AudioContext();

    SQEE_COPY_DELETE(AudioContext)
    SQEE_MOVE_DELETE(AudioContext)

    ~AudioContext();

    //----------------------------------------------------------

    /// Play a sound using the next available id.
    int32_t play_sound(const Sound& sound, SoundGroup group, float volume, bool loop);

    //----------------------------------------------------------

    /// Pause or resume an individual sound.
    void set_sound_paused(int32_t id, bool pause);

    /// Remove an individual sound.
    void stop_sound(int32_t id);

    //----------------------------------------------------------

    /// Pause or resume zero or more sounds.
    void set_sounds_paused(std::span<const int32_t> ids, bool pause);

    /// Remove zero or more sounds.
    void stop_sounds(std::span<const int32_t> ids);

    //----------------------------------------------------------

    /// Do not start sounds from one or more groups.
    void set_groups_ignored(SoundGroups groups, bool ignore);

    /// Pause sounds from one or more sound groups.
    void set_groups_paused(SoundGroups groups, bool pause);

    /// Remove all sounds in the specified groups.
    void stop_groups(SoundGroups groups);

private: //=====================================================

    struct ActiveSound
    {
        int32_t id;
        uint cursor;
        const float* frames;
        uint frameCount;
        float volume;
        SoundGroup group;
        bool loop;
        bool paused;
    };

    std::vector<ActiveSound> mActiveSounds;

    int32_t mCurrentId = -1;

    SoundGroups mIgnoredGroups {};
    SoundGroups mPausedGroups {};

    struct Implementation;
    std::unique_ptr<Implementation> impl;
    friend Implementation;

    void impl_destroy_sound(const Sound& sound);
    friend Sound;
};

} // namespace sq ##############################################################
